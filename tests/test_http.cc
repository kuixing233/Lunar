#include "../lunar/http/http.h"
#include "../lunar/log.h"

void test_request()
{
    lunar::http::HttpRequest::ptr req(new lunar::http::HttpRequest);
    req->setHeader("host", "www.sylar.top");
    req->setBody("hello lunar");

    req->dump(std::cout) << std::endl;
}

void test_response()
{
    lunar::http::HttpResponse::ptr rsp(new lunar::http::HttpResponse);
    rsp->setHeader("X-X", "sylar");
    rsp->setBody("hello lunar");
    rsp->setStatus((lunar::http::HttpStatus)400);
    rsp->setClose(false);

    rsp->dump(std::cout) << std::endl;
}

int main(int argc, char **argv)
{
    test_request();
    test_response();
    return 0;
}