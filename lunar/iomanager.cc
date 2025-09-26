#include "iomanager.h"
#include "log.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

namespace lunar
{

static lunar::Logger::ptr g_logger = LUNAR_LOG_NAME("system");

enum EpollCtlOp
{

};

static std::ostream &operator<<(std::ostream &os, const EpollCtlOp &op)
{
    switch ((int)op)
    {
#define XX(ctl)                                                                \
    case ctl:                                                                  \
        return os << #ctl;
        XX(EPOLL_CTL_ADD);
        XX(EPOLL_CTL_MOD);
        XX(EPOLL_CTL_DEL);
    default:
        return os << (int)op;
    }
#undef XX
}

static std::ostream &operator<<(std::ostream &os, EPOLL_EVENTS events)
{
    if (!events)
    {
        return os << "0";
    }
    bool first = true;
#define XX(E)                                                                  \
    if (events & E)                                                            \
    {                                                                          \
        if (!first)                                                            \
        {                                                                      \
            os << "|";                                                         \
        }                                                                      \
        os << #E;                                                              \
        first = false;                                                         \
    }
    XX(EPOLLIN);
    XX(EPOLLPRI);
    XX(EPOLLOUT);
    XX(EPOLLRDNORM);
    XX(EPOLLRDBAND);
    XX(EPOLLWRNORM);
    XX(EPOLLWRBAND);
    XX(EPOLLMSG);
    XX(EPOLLERR);
    XX(EPOLLHUP);
    XX(EPOLLRDHUP);
    XX(EPOLLONESHOT);
    XX(EPOLLET);
#undef XX
    return os;
}

IOManager::FdContext::EventContext &IOManager::FdContext::getContext(
    IOManager::Event event)
{
    switch (event)
    {
    case IOManager::READ:
        return read;
    case IOManager::WRITE:
        return write;
    default:;
    }
    throw std::invalid_argument("getContext invalid event");
}

void IOManager::FdContext::resetContext(EventContext &ctx)
{
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
    ctx.cb = nullptr;
}

void IOManager::FdContext::triggerEvent(IOManager::Event event)
{

    // 将 events 上的 event 去掉
    events = (Event)(events & ~event);
    EventContext &ctx = getContext(event);
    if (ctx.cb)
    {
        ctx.scheduler->schedule(&ctx.cb);
    }
    else
    {
        ctx.scheduler->schedule(&ctx.fiber);
    }
    ctx.scheduler = nullptr;
    return;
}

IOManager::IOManager(size_t threads, bool use_caller, const std::string &name)
    : Scheduler(threads, use_caller, name)
{
    // 创建epoll实例
    m_epfd = epoll_create(5000);

    // 创建pipe，获取m_tickleFds[2]，其中m_tickleFds[0]是管道的读端，m_tickleFds[1]是管道的写端
    int rt = pipe(m_tickleFds);

    // 注册pipe读句柄的可读事件，用于tickle调度协程，通过epoll_event.data.fd保存描述符
    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];

    // 非阻塞方式，配合边缘触发
    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);

    // 将管道的读描述符加入epoll多路复用，如果管道可读，idle中的epoll_wait会返回
    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);

    contextResize(32);

    // 这里直接开启了Schedluer，也就是说IOManager创建即可调度协程
    start();
}

IOManager::~IOManager()
{
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for (size_t i = 0; i < m_fdContexts.size(); ++i)
    {
        if (m_fdContexts[i])
        {
            delete m_fdContexts[i];
        }
    }
}

void IOManager::contextResize(size_t size)
{
    m_fdContexts.resize(size);

    for (size_t i = 0; i < m_fdContexts.size(); ++i)
    {
        if (!m_fdContexts[i])
        {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->fd = i;
        }
    }
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb)
{
    FdContext *fd_ctx = nullptr;
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_fdContexts.size() > fd)
    {
        fd_ctx = m_fdContexts[fd];
        lock.unlock();
    }
    else
    {
        lock.unlock();
        RWMutexType::WriteLock lock2(m_mutex);
        contextResize(fd * 1.5);
        fd_ctx = m_fdContexts[fd];
    }

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);

    // 这件事情已经有了，就会有错误。
    // if(LUNAR_UNLIKELY(fd_ctx->events & event)) {
    //     LUNAR_LOG_ERROR(g_logger) << "addEvent assert fd=" << fd
    //                 << " event=" << (EPOLL_EVENTS)event
    //                 << " fd_ctx.event=" << (EPOLL_EVENTS)fd_ctx->events;
    //     LUNAR_ASSERT(!(fd_ctx->events & event));
    // }

    int op = fd_ctx->events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_ctx->events | event;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt)
    {
        LUNAR_LOG_ERROR(g_logger)
            << "epoll_ctl(" << m_epfd << ", " << (EpollCtlOp)op << ", " << fd
            << ", " << (EPOLL_EVENTS)epevent.events << "):" << rt << " ("
            << errno << ") (" << strerror(errno)
            << ") fd_ctx->events = " << (EPOLL_EVENTS)fd_ctx->events;
        return -1;
    }

    ++m_pendingEventCount;
    fd_ctx->events = (Event)(fd_ctx->events | event);
    FdContext::EventContext &event_ctx = fd_ctx->getContext(event);
    // LUNAR_ASSERT(!event_ctx.scheduler
    //             && !event_ctx.fiber
    //             && !event_ctx.cb);

    event_ctx.scheduler = Scheduler::GetThis();
    if (cb)
    {
        event_ctx.cb.swap(cb);
    }
    else
    {
        // 有待商榷
        event_ctx.fiber = Fiber::GetThis();
        // LUNAR_ASSERT2(event_ctx.fiber->getState() == Fiber::EXEC
        //               ,"state=" << event_ctx.fiber->getState());
    }
    return 0;
}

bool IOManager::delEvent(int fd, Event event)
{
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_fdContexts.size() <= fd)
    {
        return false;
    }
    FdContext *fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    // if(LUNAR_UNLIKELY(!(fd_ctx->events & event))) {
    //     return false;
    // }

    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt)
    {
        // LUNAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
        //     << (EpollCtlOp)op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
        //     << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

    --m_pendingEventCount;
    fd_ctx->events = new_events;
    FdContext::EventContext &event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);
    return true;
}

bool IOManager::cancelEvent(int fd, Event event)
{
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_fdContexts.size() <= fd)
    {
        return false;
    }
    FdContext *fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    // if(LUNAR_UNLIKELY(!(fd_ctx->events & event))) {
    //     return false;
    // }

    Event new_events = (Event)(fd_ctx->events & ~event);
    int op = new_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_events;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt)
    {
        // LUNAR_LOG_ERROR(g_logger) << "epoll_ctl(" << m_epfd << ", "
        //     << (EpollCtlOp)op << ", " << fd << ", " << (EPOLL_EVENTS)epevent.events << "):"
        //     << rt << " (" << errno << ") (" << strerror(errno) << ")";
        return false;
    }

    fd_ctx->triggerEvent(event);
    --m_pendingEventCount;
    return true;
}

bool IOManager::cancelAll(int fd)
{
    RWMutexType::ReadLock lock(m_mutex);
    if ((int)m_fdContexts.size() <= fd)
    {
        return false;
    }
    FdContext *fd_ctx = m_fdContexts[fd];
    lock.unlock();

    FdContext::MutexType::Lock lock2(fd_ctx->mutex);
    if (!fd_ctx->events)
    {
        return false;
    }

    int op = EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = 0;
    epevent.data.ptr = fd_ctx;

    int rt = epoll_ctl(m_epfd, op, fd, &epevent);
    if (rt)
    {
        LUNAR_LOG_ERROR(g_logger)
            << "epoll_ctl(" << m_epfd << ", " << (EpollCtlOp)op << ", " << fd
            << ", " << (EPOLL_EVENTS)epevent.events << "):" << rt << " ("
            << errno << ") (" << strerror(errno) << ")";
        return false;
    }

    if (fd_ctx->events & READ)
    {
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
    }
    if (fd_ctx->events & WRITE)
    {
        fd_ctx->triggerEvent(WRITE);
        --m_activeThreadCount;
    }

    // LUNAR_ASSERT(fd_ctx->events == 0);
    return true;
}

IOManager *IOManager::GetThis()
{
    return dynamic_cast<IOManager *>(Scheduler::GetThis());
}

void IOManager::tickle()
{
    // std::cout << "[INFO]   IOManager::tickle" << std::endl;
    if (!hasIdleThreads())
    {
        return;
    }
    int rt = write(m_tickleFds[1], "T", 1);
}

bool IOManager::stopping(uint64_t &timeout)
{
    timeout = getNextTimer();
    return timeout == ~0ull && m_pendingEventCount == 0 &&
           Scheduler::stopping();
}

bool IOManager::stopping()
{
    uint64_t timeout = 0;
    return stopping(timeout);
}

void IOManager::idle()
{
    LUNAR_LOG_DEBUG(g_logger) << "idle";

    // 一次epoll_wait最多检测256个就绪事件，如果就绪事件超过了这个数，那么会在下轮epoll_wati继续处理
    const uint64_t MAX_EVENTS = 256;
    epoll_event *events = new epoll_event[MAX_EVENTS]();
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event *ptr) {
        delete[] ptr;
    });

    while (true)
    {
        uint64_t next_timeout = 0;
        if (stopping(next_timeout))
        {
            LUNAR_LOG_INFO(g_logger)
                << "name = " << getName() << " idle stopping exit";
            break;
        }

        // 阻塞在epoll_wait上，等待事件发生
        int rt = 0;
        do
        {
            // 默认超时时间5秒，如果下一个定时器的超时时间大于3秒，仍以3秒来计算超时，避免定时器超时时间太大时，epoll_wait一直阻塞
            static const int MAX_TIMEOUT = 3000;
            if (next_timeout != ~0ull)
            {
                next_timeout = (int)next_timeout > MAX_TIMEOUT ? MAX_TIMEOUT
                                                               : next_timeout;
            }
            else
            {
                next_timeout = MAX_TIMEOUT;
            }
            rt = epoll_wait(m_epfd, events, MAX_EVENTS, (int)next_timeout);
            // LUNAR_LOG_INFO(g_logger) << "ep_wait return";
            if (rt < 0 && errno == EINTR)
            {
                // 这里为空，是因为有可能是超时时间到了
                LUNAR_LOG_DEBUG(g_logger) << "ep_wiat return < 0";
            }
            else
            {
                break;
            }
        } while (true);

        // 这里是定时器的一些回调，在这里将已经到期的回调一股脑调度上去
        std::vector<std::function<void()>> cbs;
        listExpiredCb(cbs);
        // LUNAR_LOG_DEBUG(g_logger) << "+++++++++++++";
        if (!cbs.empty())
        {
            // LUNAR_LOG_DEBUG(g_logger) << "schedule listExpiredCb";
            // LUNAR_LOG_DEBUG(g_logger) << cbs.size();
            schedule(cbs.begin(), cbs.end());
            cbs.clear();
        }

        // LUNAR_LOG_DEBUG(g_logger) << "---------";
        // 遍历所有发生的事件，根据epoll_event的私有指针找到对应的FdContext，进行事件处理
        for (int i = 0; i < rt; ++i)
        {
            epoll_event &event = events[i];

            // ticklefd[0]用于通知协程调度，这时只需要把管道里的内容读完即可，本轮idle结束Scheduler::run会重新执行协程调度
            if (event.data.fd == m_tickleFds[0])
            {
                uint8_t dummy[256];
                while (read(m_tickleFds[0], dummy, sizeof(dummy)) > 0)
                    ;
                continue;
            }

            // 通过epoll_event的私有指针获取FdContext
            FdContext *fd_ctx = (FdContext *)event.data.ptr;
            FdContext::MutexType::Lock lock(fd_ctx->mutex);

            /**
             * EPOLLERR: 出错，比如写读端已经关闭的pipe
             * EPOLLHUP: 套接字对端关闭
             * 出现这两种事件，应该同时触发fd的读和写事件，否则有可能出现注册的事件永远执行不到的情况
             */
            if (event.events & (EPOLLERR | EPOLLHUP))
            {
                event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->events;
            }
            int real_events = NONE;
            if (event.events & EPOLLIN)
            {
                real_events |= READ;
            }
            if (event.events & EPOLLOUT)
            {
                real_events |= WRITE;
            }

            if ((fd_ctx->events & real_events) == NONE)
            {
                continue;
            }

            // 剔除已经发生的事件，将剩下的事件重新加入epoll_wait，
            // 如果剩下的事件为0，表示这个fd已经不需要关注了，直接从epoll中删除
            int left_events = (fd_ctx->events & ~real_events);
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;

            int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if (rt2)
            {
                LUNAR_LOG_ERROR(g_logger)
                    << "epoll_ctl(" << m_epfd << ", " << (EpollCtlOp)op << ", "
                    << fd_ctx->fd << ", " << (EPOLL_EVENTS)event.events
                    << "):" << rt2 << " (" << errno << ") (" << strerror(errno)
                    << ")";
                continue;
            }

            // 处理已经发生的事件，也就是让调度器调度指定的函数或协程
            if (real_events & READ)
            {
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }
            if (real_events & WRITE)
            {
                fd_ctx->triggerEvent(WRITE);
                --m_pendingEventCount;
            }
        }

        /**
         * 一旦处理完所有的事件，idle协程yield，这样可以让调度协程(Scheduler::run)重新检查是否有新任务要调度
         * 上面triggerEvent实际也只是把对应的fiber重新加入调度，要执行的话还要等idle协程退出
         */
        Fiber::ptr cur = Fiber::GetThis();
        auto raw_ptr = cur.get();
        cur.reset();

        raw_ptr->swapOut();
    }
    LUNAR_LOG_INFO(g_logger) << "idle over";
}

void IOManager::onTimerInsertedAtFront()
{
    tickle();
}

} // namespace lunar