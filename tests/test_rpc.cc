#include "../lunar/lunar.h"
lunar::Logger::ptr g_logger = LUNAR_LOG_ROOT();


void run()
{
    lunar::rpc::RpcProvider rpc;
    rpc.run();
    // lunar::IPAddress::ptr addr = lunar::IPAddress::Create("127.0.0.1", 8000);

    // lunar::TcpServer::ptr server(new lunar::TcpServer);
    // server->bind(addr);

    // sleep(2);

    // server->start();
}

int main(int argc, char **argv)
{

    lunar::IOManager iom(2);
    iom.schedule(run);
    // iom.stop();
    // run();
    return 0;
}