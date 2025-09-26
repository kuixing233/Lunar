#include "../lunar/hook.h"
#include "../lunar/iomanager.h"
#include "../lunar/log.h"
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

lunar::Logger::ptr g_logger = LUNAR_LOG_ROOT();

void test_sleep()
{
    lunar::IOManager iom(1);
    iom.schedule([]() {
        sleep(2);
        LUNAR_LOG_INFO(g_logger) << "sleep 2";
    });

    iom.schedule([]() {
        sleep(3);
        LUNAR_LOG_INFO(g_logger) << "sleep 3";
    });
    LUNAR_LOG_INFO(g_logger) << "test_sleep";
}

void test_sock()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    inet_pton(AF_INET, "115.239.210.27", &addr.sin_addr.s_addr);

    LUNAR_LOG_INFO(g_logger) << "begin connect";
    int rt = connect(sock, (const sockaddr *)&addr, sizeof(addr));
    LUNAR_LOG_INFO(g_logger) << "connect rt=" << rt << " errno=" << errno;

    if (rt)
    {
        return;
    }

    const char data[] = "GET / HTTP/1.0\r\n\r\n";
    rt = send(sock, data, sizeof(data), 0);
    LUNAR_LOG_INFO(g_logger) << "send rt=" << rt << " errno=" << errno;

    if (rt <= 0)
    {
        return;
    }

    std::string buff;
    buff.resize(4096);

    rt = recv(sock, &buff[0], buff.size(), 0);
    LUNAR_LOG_INFO(g_logger) << "recv rt=" << rt << " errno=" << errno;

    if (rt <= 0)
    {
        return;
    }

    buff.resize(rt);
    LUNAR_LOG_INFO(g_logger) << buff;
}

int main(int argc, char **argv)
{
    test_sleep();
    // lunar::IOManager iom;
    // iom.schedule(test_sock);
    return 0;
}