#ifndef __LUNAR_HTTP_HTTP_SERVER_H__
#define __LUNAR_HTTP_HTTP_SERVER_H__

#include "../tcp_server.h"
#include "http_session.h"
#include "servlet.h"

namespace lunar
{

namespace http
{

class HttpServer : public TcpServer
{
public:
    typedef std::shared_ptr<HttpServer> ptr;
    HttpServer(bool keepalive = false,
               lunar::IOManager *worker = lunar::IOManager::GetThis(),
               lunar::IOManager *accept_worker = lunar::IOManager::GetThis());

    ServletDispatch::ptr getServletDispatch() const
    {
        return m_dispatch;
    }
    void setServletDispatch(ServletDispatch::ptr v)
    {
        m_dispatch = v;
    }

protected:
    void handleClient(Socket::ptr client) override;

private:
    bool m_isKeepalive;
    ServletDispatch::ptr m_dispatch;
};

} // namespace http

} // namespace lunar

#endif