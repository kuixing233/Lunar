#include "../lunar/bytearray.h"
#include "../lunar/iomanager.h"
#include "../lunar/log.h"
#include "../lunar/tcp_server.h"

static lunar::Logger::ptr g_logger = LUNAR_LOG_ROOT();

class EchoServer : public lunar::TcpServer
{
public:
    EchoServer(int type);
    void handleClient(lunar::Socket::ptr client);

private:
    int m_type = 0;
};

EchoServer::EchoServer(int type) : m_type(type)
{
}

void EchoServer::handleClient(lunar::Socket::ptr client)
{
    LUNAR_LOG_INFO(g_logger) << "handleClient " << *client;
    lunar::ByteArray::ptr ba(new lunar::ByteArray);
    while (true)
    {
        ba->clear();
        std::vector<iovec> iovs;
        ba->getWriteBuffers(iovs, 1024);

        int rt = client->recv(&iovs[0], iovs.size());
        if (rt == 0)
        {
            LUNAR_LOG_INFO(g_logger) << "client close: " << *client;
            break;
        }
        else if (rt < 0)
        {
            LUNAR_LOG_INFO(g_logger)
                << "client error rt=" << rt << " errno=" << errno
                << " errstr=" << strerror(errno);
            break;
        }
        ba->setPosition(ba->getPosition() + rt);
        ba->setPosition(0);
        if (m_type == 1)
        { // text
            std::cout << ba->toString();
        }
        else
        {
            std::cout << ba->toHexString();
        }
    }
}

int type = 1;

void run()
{
    EchoServer::ptr es(new EchoServer(type));
    auto addr = lunar::Address::LookupAny("0.0.0.0:8022");
    while (!es->bind(addr))
    {
        sleep(2);
    }
    es->start();
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        LUNAR_LOG_INFO(g_logger)
            << "used as[" << argv[0] << " -t] or [" << argv[0] << " -b]";
        return 0;
    }

    if (!strcmp(argv[1], "-b"))
    {
        type = 2;
    }

    lunar::IOManager iom(2);
    LUNAR_LOG_INFO(g_logger) << "type = " << type;
    iom.schedule(run);
    return 0;
}