#include "../lunar/iomanager.h"
#include "../lunar/socket.h"

static lunar::Logger::ptr g_looger = LUNAR_LOG_ROOT();

void test_socket()
{
    //std::vector<lunar::Address::ptr> addrs;
    //lunar::Address::Lookup(addrs, "www.baidu.com", AF_INET);
    //lunar::IPAddress::ptr addr;
    //for(auto& i : addrs) {
    //    LUNAR_LOG_INFO(g_looger) << i->toString();
    //    addr = std::dynamic_pointer_cast<lunar::IPAddress>(i);
    //    if(addr) {
    //        break;
    //    }
    //}
    lunar::IPAddress::ptr addr =
        lunar::Address::LookupAnyIPAddress("www.bilibili.com");
    // lunar::IPAddress::ptr addr = lunar::Address::LookupAnyIPAddress("www.mihoyo.com");
    if (addr)
    {
        LUNAR_LOG_INFO(g_looger) << "get address: " << addr->toString();
    }
    else
    {
        LUNAR_LOG_ERROR(g_looger) << "get address fail";
        return;
    }

    lunar::Socket::ptr sock = lunar::Socket::CreateTCP(addr);
    addr->setPort(80);
    LUNAR_LOG_INFO(g_looger) << "addr=" << addr->toString();
    if (!sock->connect(addr))
    {
        LUNAR_LOG_ERROR(g_looger) << "connect " << addr->toString() << " fail";
        return;
    }
    else
    {
        LUNAR_LOG_INFO(g_looger)
            << "connect " << addr->toString() << " connected";
    }

    const char buff[] = "GET / HTTP/1.0\r\n\r\n";
    int rt = sock->send(buff, sizeof(buff));
    if (rt <= 0)
    {
        LUNAR_LOG_INFO(g_looger) << "send fail rt=" << rt;
        return;
    }

    std::string buffs;
    buffs.resize(4096);
    rt = sock->recv(&buffs[0], buffs.size());

    if (rt <= 0)
    {
        LUNAR_LOG_INFO(g_looger) << "recv fail rt=" << rt;
        return;
    }

    buffs.resize(rt);
    LUNAR_LOG_INFO(g_looger) << "ret msg is " << buffs;
}

void test2()
{
    lunar::IPAddress::ptr addr =
        lunar::Address::LookupAnyIPAddress("www.baidu.com:80");
    if (addr)
    {
        LUNAR_LOG_INFO(g_looger) << "get address: " << addr->toString();
    }
    else
    {
        LUNAR_LOG_ERROR(g_looger) << "get address fail";
        return;
    }

    lunar::Socket::ptr sock = lunar::Socket::CreateTCP(addr);
    if (!sock->connect(addr))
    {
        LUNAR_LOG_ERROR(g_looger) << "connect " << addr->toString() << " fail";
        return;
    }
    else
    {
        LUNAR_LOG_INFO(g_looger)
            << "connect " << addr->toString() << " connected";
    }

    uint64_t ts = lunar::GetCurrentUS();
    for (size_t i = 0; i < 10000000000ul; ++i)
    {
        if (int err = sock->getError())
        {
            LUNAR_LOG_INFO(g_looger)
                << "err=" << err << " errstr=" << strerror(err);
            break;
        }

        //struct tcp_info tcp_info;
        //if(!sock->getOption(IPPROTO_TCP, TCP_INFO, tcp_info)) {
        //    LUNAR_LOG_INFO(g_looger) << "err";
        //    break;
        //}
        //if(tcp_info.tcpi_state != TCP_ESTABLISHED) {
        //    LUNAR_LOG_INFO(g_looger)
        //            << " state=" << (int)tcp_info.tcpi_state;
        //    break;
        //}
        static int batch = 10000000;
        if (i && (i % batch) == 0)
        {
            uint64_t ts2 = lunar::GetCurrentUS();
            LUNAR_LOG_INFO(g_looger)
                << "i=" << i << " used: " << ((ts2 - ts) * 1.0 / batch)
                << " us";
            ts = ts2;
        }
    }
}

void test_server()
{
    LUNAR_LOG_INFO(g_looger) << "server start";
    lunar::IPAddress::ptr addr = lunar::IPAddress::Create("127.0.0.1", 9527);
    if (addr)
    {
        LUNAR_LOG_INFO(g_looger) << "get address: " << addr->toString();
    }
    else
    {
        LUNAR_LOG_ERROR(g_looger) << "get address fail";
        return;
    }

    lunar::Socket::ptr server_sock = lunar::Socket::CreateTCP(addr);

    server_sock->bind(addr);

    if (!server_sock->isValid())
    {
        LUNAR_LOG_INFO(g_looger) << "server sock creat fail";
        return;
    }

    server_sock->listen(10);

    LUNAR_LOG_INFO(g_looger) << "listen succ";

    lunar::Socket::ptr client_sock = server_sock->accept();

    if (client_sock->isValid())
    {
        LUNAR_LOG_INFO(g_looger) << "accept succ";
    }

    lunar::IOManager::GetThis()->addEvent(
        client_sock->getSocket(),
        lunar::IOManager::READ,
        [client_sock]() {
            LUNAR_LOG_INFO(g_looger) << "read callback";
            sleep(5);
            std::string buffer;
            buffer.resize(1024);
            client_sock->recv(&buffer[0], buffer.size());
            LUNAR_LOG_INFO(g_looger)
                << "from client: " << buffer.size() << buffer;
        });

    // sleep(5);
    // std::string buffer;
    // buffer.resize(1024);
    // client_sock->recv(&buffer[0], buffer.size());
    // LUNAR_LOG_INFO(g_looger) << "from client: " << buffer.size() << buffer;
}

void test_client()
{
    LUNAR_LOG_INFO(g_looger) << "client start";
    lunar::IPAddress::ptr addr = lunar::IPAddress::Create("127.0.0.1", 8000);
    if (addr)
    {
        LUNAR_LOG_INFO(g_looger) << "get address: " << addr->toString();
    }
    else
    {
        LUNAR_LOG_ERROR(g_looger) << "get address fail";
        return;
    }

    lunar::Socket::ptr client_sock = lunar::Socket::CreateTCPSocket();
    if (!client_sock->connect(addr))
    {
        LUNAR_LOG_ERROR(g_looger) << "connect " << addr->toString() << " fail";
        return;
    }
    else
    {
        LUNAR_LOG_INFO(g_looger)
            << "connect " << addr->toString() << " connected";
    }

    const char buff[] = "hello server";
    int rt = client_sock->send(buff, sizeof(buff));
    if (rt <= 0)
    {
        LUNAR_LOG_INFO(g_looger) << "send fail rt=" << rt;
        return;
    }
    sleep(2);

    bool isConnection = client_sock->isConnected();
    if (isConnection)
    {
        LUNAR_LOG_INFO(g_looger) << "connectioned";
    }

    // std::string buffer;
    // buffer.resize(1024);
    // client_sock->recv(&buffer[0], buffer.size());
    // LUNAR_LOG_INFO(g_looger) << "from client: " << buffer.size() << buffer;

    sleep(2);
    rt = client_sock->send(buff, sizeof(buff));
    if (rt <= 0)
    {
        LUNAR_LOG_INFO(g_looger) << "send fail rt=" << rt;
        return;
    }

    sleep(2);
    std::string buffe;
    buffe.resize(1024);
    client_sock->recv(&buffe[0], buffe.size());
    LUNAR_LOG_INFO(g_looger) << "from client: " << buffe.size() << buffe;

    LUNAR_LOG_INFO(g_looger) << "over";
}

int main(int argc, char **argv)
{
    // lunar::IOManager iom(2);
    // iom.schedule(&test_socket);
    // iom.schedule(&test2);
    // iom.schedule(&test_server);
    // sleep(2);
    // iom.schedule(&test_client);
    test_client();
    // test_client();
    // test_server();
    return 0;
}