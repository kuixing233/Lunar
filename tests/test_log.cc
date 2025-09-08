#include "../lunar/log.h"

#include <unistd.h>

lunar::Logger::ptr g_logger(new lunar::Logger);

int main(int argc, char** argv) {
    g_logger->addAppender(lunar::LogAppender::ptr(new lunar::StdoutLogAppender));
    ALPHA_LOG_FATAL(g_logger) << "fatal msg";
    ALPHA_LOG_ERROR(g_logger) << "err msg";
    ALPHA_LOG_INFO(g_logger) << "info msg";
    ALPHA_LOG_DEBUG(g_logger) << "debug msg";

    ALPHA_LOG_INFO(g_logger) << "==========================================";

}