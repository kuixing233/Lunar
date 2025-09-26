#include "../lunar/fiber.h"
#include "../lunar/thread.h"

lunar::Logger::ptr g_logger = LUNAR_LOG_ROOT();

void run_in_fiber()
{
    LUNAR_LOG_INFO(g_logger) << "run_in_fiber begin";
    lunar::Fiber::YieldToHold();
    LUNAR_LOG_INFO(g_logger) << "run_in_fiber begin";
    lunar::Fiber::YieldToHold();
}

void test_fiber()
{
    LUNAR_LOG_INFO(g_logger) << "main begin";
    {
        lunar::Fiber::GetThis();
        LUNAR_LOG_INFO(g_logger) << "main begin";
        lunar::Fiber::ptr fiber(new lunar::Fiber(run_in_fiber));
        fiber->swapIn();
        LUNAR_LOG_INFO(g_logger) << "main after swapIn";
        fiber->swapIn();
        LUNAR_LOG_INFO(g_logger) << "main after end";
        fiber->swapIn();
    }
    LUNAR_LOG_INFO(g_logger) << "main after end2";
}

int main(int argc, char **argv)
{
    lunar::Thread::SetName("main");

    std::vector<lunar::Thread::ptr> thrs;

    for (int i = 0; i < 5; i++)
    {
        thrs.push_back(lunar::Thread::ptr(
            new lunar::Thread(&test_fiber, "name_" + std::to_string(i))));
    }
    for (auto i : thrs)
    {
        i->join();
    }
    // test_fiber();

    return 0;
}