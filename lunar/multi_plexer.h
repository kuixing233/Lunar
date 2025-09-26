#ifndef __LUNAR_MULTI_PLEXER_H__
#define __LUNAR_MULTI_PLEXER_H__

#include <memory>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <vector>

namespace lunar
{

class Multiplexer
{
public:
    typedef std::shared_ptr<Multiplexer> ptr;

    virtual ~Multiplexer()
    {
    }
    virtual void addFileDescriptor(int fd) = 0;
    virtual void removeFileDescriptor(int fd) = 0;
    virtual std::vector<int> waitForEvents() = 0;
};

class SelectMultiplexer : public Multiplexer
{
public:
    typedef std::shared_ptr<SelectMultiplexer> ptr;

    SelectMultiplexer();
    ~SelectMultiplexer();
    void addFileDescriptor(int fd) override;
    void removeFileDescriptor(int fd) override;
    std::vector<int> waitForEvents() override;

private:
    fd_set m_readfds;
    fd_set m_writefds;
    fd_set m_errorfds;
    int m_maxfd;
};

class PollMultiplexer : public Multiplexer
{
public:
    typedef std::shared_ptr<PollMultiplexer> ptr;

    PollMultiplexer();
    ~PollMultiplexer();
    void addFileDescriptor(int fd) override;
    void removeFileDescriptor(int fd) override;
    std::vector<int> waitForEvents() override;

private:
    std::vector<pollfd> m_pollfds;
};

class EpollMultiplexer : public Multiplexer
{
public:
    typedef std::shared_ptr<EpollMultiplexer> ptr;

    EpollMultiplexer();
    ~EpollMultiplexer();
    void addFileDescriptor(int fd) override;
    void removeFileDescriptor(int fd) override;
    std::vector<int> waitForEvents() override;

private:
    int m_epollfd;
};

} // namespace lunar

#endif