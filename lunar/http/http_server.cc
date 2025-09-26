#include "http_server.h"
#include "../log.h"

namespace lunar
{

namespace http
{

static lunar::Logger::ptr g_logger = LUNAR_LOG_NAME("system");

HttpServer::HttpServer(bool keepalive,
                       lunar::IOManager *worker,
                       lunar::IOManager *accept_worker)
    : TcpServer(worker, accept_worker), m_isKeepalive(keepalive)
{
    m_dispatch.reset(new ServletDispatch);
}


void HttpServer::handleClient(Socket::ptr client)
{
    HttpSession::ptr session(new HttpSession(client));
    do
    {
        auto req = session->recvRequest();
        if (!req)
        {
            LUNAR_LOG_WARN(g_logger)
                << "recv http request fail, errno=" << errno
                << " errstr=" << strerror(errno) << " client:" << *client;
            break;
        }

        HttpResponse::ptr rsp(
            new HttpResponse(req->getVersion(),
                             req->isClose() || !m_isKeepalive));
        m_dispatch->handle(req, rsp, session);
        // rsp->setBody("hello lunar");

        // LUNAR_LOG_INFO(g_logger) << "request:" << std::endl
        //     << *req;

        // LUNAR_LOG_INFO(g_logger) << "response:" << std::endl
        //     << *rsp;
        session->sendResponse(rsp);
    } while (m_isKeepalive);
    session->close();
}

} // namespace http

} // namespace lunar