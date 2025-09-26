#include "../lunar/log.h"
#include "../lunar/scheduler.h"

static lunar::Logger::ptr g_logger = LUNAR_LOG_ROOT();

void test_fiber()
{
    static int s_count = 5;
    LUNAR_LOG_INFO(g_logger) << "test in fiber s_count = " << s_count;

    sleep(1);
    if (--s_count >= 0)
    {
        lunar::Scheduler::GetThis()->schedule(&test_fiber,
                                              lunar::GetThreadId());
    }
}


int main(int argc, char **argv)
{
    LUNAR_LOG_INFO(g_logger) << "main";
    lunar::Scheduler sc(3, false, "test");
    sc.start();
    sleep(2);
    LUNAR_LOG_INFO(g_logger) << "schedule";
    sc.schedule(&test_fiber);
    sc.stop();
    LUNAR_LOG_INFO(g_logger) << "over";
    return 0;

    return 0;
}