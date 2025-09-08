#include "../lunar/http/http_connection.h"
#include <iostream>
#include <fstream>
#include "../lunar/log.h"
#include "../lunar/address.h"
#include "../lunar/lunar.h"

static lunar::Logger::ptr g_logger = ALPHA_LOG_ROOT();

void run() {
    lunar::Address::ptr addr = lunar::Address::LookupAnyIPAddress("www.sylar.top:80");
    if(!addr) {
        ALPHA_LOG_INFO(g_logger) << "get addr error";
        return;
    }

    lunar::Socket::ptr sock = lunar::Socket::CreateTCP(addr);
    bool rt = sock->connect(addr);
    if(!rt) {
        ALPHA_LOG_INFO(g_logger) << "connect " << *addr << " failed";
    }

    lunar::http::HttpConnection::ptr conn(new lunar::http::HttpConnection(sock));

    lunar::http::HttpRequest::ptr req(new lunar::http::HttpRequest);
    req->setPath("/blog/");
    req->setHeader("host", "www.sylar.top");
    ALPHA_LOG_INFO(g_logger) << "req:" << std::endl
        << *req;

    conn->sendRequest(req);
    ALPHA_LOG_INFO(g_logger) << "1";
    auto rsp = conn->recvResponse();
    ALPHA_LOG_INFO(g_logger) << "2";
    if(!rsp) {
        ALPHA_LOG_INFO(g_logger) << "recv response error";
        return;
    }
    ALPHA_LOG_INFO(g_logger) << "3";
    ALPHA_LOG_INFO(g_logger) << "rsp:" << std::endl
        << *rsp;
    ALPHA_LOG_INFO(g_logger) << "4";

    std::ofstream ofs("rsp.dat");
    ofs << *rsp;
}

int main(int argc, char** argv) {
    lunar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}