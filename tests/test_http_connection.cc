#include "../lunar/address.h"
#include "../lunar/http/http_connection.h"
#include "../lunar/log.h"
#include "../lunar/lunar.h"
#include <fstream>
#include <iostream>

static lunar::Logger::ptr g_logger = LUNAR_LOG_ROOT();

void run()
{
    lunar::Address::ptr addr =
        lunar::Address::LookupAnyIPAddress("www.sylar.top:80");
    if (!addr)
    {
        LUNAR_LOG_INFO(g_logger) << "get addr error";
        return;
    }

    lunar::Socket::ptr sock = lunar::Socket::CreateTCP(addr);
    bool rt = sock->connect(addr);
    if (!rt)
    {
        LUNAR_LOG_INFO(g_logger) << "connect " << *addr << " failed";
    }

    lunar::http::HttpConnection::ptr conn(
        new lunar::http::HttpConnection(sock));

    lunar::http::HttpRequest::ptr req(new lunar::http::HttpRequest);
    req->setPath("/blog/");
    req->setHeader("host", "www.sylar.top");
    LUNAR_LOG_INFO(g_logger) << "req:" << std::endl << *req;

    conn->sendRequest(req);
    LUNAR_LOG_INFO(g_logger) << "1";
    auto rsp = conn->recvResponse();
    LUNAR_LOG_INFO(g_logger) << "2";
    if (!rsp)
    {
        LUNAR_LOG_INFO(g_logger) << "recv response error";
        return;
    }
    LUNAR_LOG_INFO(g_logger) << "3";
    LUNAR_LOG_INFO(g_logger) << "rsp:" << std::endl << *rsp;
    LUNAR_LOG_INFO(g_logger) << "4";

    std::ofstream ofs("rsp.dat");
    ofs << *rsp;
}

int main(int argc, char **argv)
{
    lunar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}