#include "../lunar/http/http_server.h"
#include "../lunar/log.h"

static lunar::Logger::ptr g_logger = ALPHA_LOG_ROOT();

int run() {
    lunar::http::HttpServer::ptr server(new lunar::http::HttpServer);
    lunar::Address::ptr addr = lunar::Address::LookupAnyIPAddress("0.0.0.0:8022");
    while(!server->bind(addr)) {
        sleep(2);
    }
    auto sd = server->getServletDispatch();
    sd->addServlet("/lunar/xx", [](lunar::http::HttpRequest::ptr req
                ,lunar::http::HttpResponse::ptr rsp
                ,lunar::http::HttpSession::ptr session) {
            rsp->setBody(req->toString());
            return 0;
    });

    sd->addGlobServlet("/lunar/*", [](lunar::http::HttpRequest::ptr req
                ,lunar::http::HttpResponse::ptr rsp
                ,lunar::http::HttpSession::ptr session) {
            rsp->setBody("Glob:\r\n" + req->toString());
            return 0;
    });
    server->start();
}

int main(int argc, char** argv) {
    lunar::IOManager iom(2);
    iom.schedule(run);
    return 0;
}