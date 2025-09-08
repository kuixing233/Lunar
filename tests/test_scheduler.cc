#include "../lunar/scheduler.h"
#include "../lunar/log.h"

static lunar::Logger::ptr g_logger = ALPHA_LOG_ROOT();

void test_fiber() {
    static int s_count = 5;
    ALPHA_LOG_INFO(g_logger) << "test in fiber s_count = " << s_count;

    sleep(1);
    if(--s_count >= 0) {
        lunar::Scheduler::GetThis()->schedule(&test_fiber, lunar::GetThreadId());
    }
}


int main(int argc, char** argv) {
    ALPHA_LOG_INFO(g_logger) << "main";
    lunar::Scheduler sc(3, false, "test");
    sc.start();
    sleep(2);
    ALPHA_LOG_INFO(g_logger) << "schedule";
    sc.schedule(&test_fiber);
    sc.stop();
    ALPHA_LOG_INFO(g_logger) << "over";
    return 0;

    return 0;
}