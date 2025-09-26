#include "../lunar/fiber_sync.h"
#include "../lunar/lunar.h"

static lunar::Logger::ptr g_logger = LUNAR_LOG_ROOT();

lunar::FiberMutex fm;

void test1()
{
    fm.lock();
    LUNAR_LOG_INFO(g_logger) << "test1 is lock";
    sleep(2);
    // lunar::Fiber::YieldToHold();
    fm.unlock();
}

void test2()
{
    fm.lock();
    LUNAR_LOG_INFO(g_logger) << "test2 is lock";
}

int main(int argc, char **argv)
{
    lunar::Scheduler scheduler(1, true, "test");

    scheduler.start();
    scheduler.schedule(&test1);
    scheduler.schedule(&test2);

    scheduler.stop();

    return 0;
}