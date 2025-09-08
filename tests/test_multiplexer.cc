#include "../lunar/multi_plexer.h"
#include <iostream>
#include "../lunar/log.h"
#include "../lunar/iomanager.h"

static lunar::Logger::ptr g_logger = ALPHA_LOG_ROOT();

void test_select() {
    lunar::SelectMultiplexer multiplexer;

    int fd1 = 0; // Replace 0 with actual file descriptor for stdin
    int fd2 = 1; // Replace 1 with actual file descriptor for stdout

    multiplexer.addFileDescriptor(fd1);

    ALPHA_LOG_INFO(g_logger) << "Enter a message: ";
    

    while(true) {
        std::vector<int> ready_fds = multiplexer.waitForEvents();

        for(int fd : ready_fds) {
            if(fd == fd1) {
                std::string message;
                std::getline(std::cin, message);

                multiplexer.addFileDescriptor(fd2);
                ALPHA_LOG_INFO(g_logger) << "Received message: " << message;
            } else if(fd == fd2) {
                ALPHA_LOG_INFO(g_logger) << "Ready to write to stdout.";
            }
        }
    }
}

void test_poll() {
    lunar::PollMultiplexer multiplexer;

    int fd1 = 0; // Replace 0 with actual file descriptor for stdin
    int fd2 = 1; // Replace 1 with actual file descriptor for stdout

    multiplexer.addFileDescriptor(fd1);
    multiplexer.addFileDescriptor(fd2);

    while (true) {
        std::vector<int> ready_fds = multiplexer.waitForEvents();

        for (int fd : ready_fds) {
            if (fd == fd1) {
                std::string message;
                std::getline(std::cin, message);
                ALPHA_LOG_INFO(g_logger) << "Received message: " << message;
            } else if(fd == fd2) {
                ALPHA_LOG_INFO(g_logger) << "Ready to write to stdout.";
            }
        }
    }
}

void test_epoll() {
    lunar::EpollMultiplexer multiplexer;

    int fd1 = 0; // Replace 0 with actual file descriptor for stdin
    int fd2 = 1; // Replace 1 with actual file descriptor for stdout

    multiplexer.addFileDescriptor(fd1);
    multiplexer.addFileDescriptor(fd2);

    while (true) {
        std::vector<int> ready_fds = multiplexer.waitForEvents();

        for (int fd : ready_fds) {
            if (fd == fd1) {
                std::string message;
                std::getline(std::cin, message);
                ALPHA_LOG_INFO(g_logger) << "Received message: " << message;
            } else if(fd == fd2) {
                ALPHA_LOG_INFO(g_logger) << "Ready to write to stdout.";
            }
        }
    }
}

int main(int argc, char** argv) {
    lunar::IOManager iom(1);
    // iom.schedule(test_select);
    iom.schedule(test_epoll);
    return 0;
}