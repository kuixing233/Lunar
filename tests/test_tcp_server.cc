#include "../lunar/iomanager.h"
#include "../lunar/log.h"
#include "../lunar/tcp_server.h"

lunar::Logger::ptr g_logger = LUNAR_LOG_ROOT();

void run()
{
    auto addr = lunar::Address::LookupAny("127.0.0.1:8000");
    //auto addr2 = lunar::UnixAddress::ptr(new lunar::UnixAddress("/tmp/unix_addr"));
    std::vector<lunar::Address::ptr> addrs;
    addrs.push_back(addr);
    //addrs.push_back(addr2);

    lunar::TcpServer::ptr tcp_server(new lunar::TcpServer);
    std::vector<lunar::Address::ptr> fails;
    while (!tcp_server->bind(addrs, fails))
    {
        sleep(2);
    }
    LUNAR_LOG_DEBUG(g_logger) << "statr";
    tcp_server->start();
}
int main(int argc, char **argv)
{
    lunar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}