#ifndef __LUNAR_HTTP_SERVLET_H__
#define __LUNAR_HTTP_SERVLET_H__

#include "../thread.h"
#include "http.h"
#include "http_session.h"
#include <functional>
#include <memory>
#include <stdint.h>
#include <string>
#include <unordered_map>

namespace lunar
{

namespace http
{

class Servlet
{
public:
    typedef std::shared_ptr<Servlet> ptr;
    Servlet(const std::string &name) : m_name(name)
    {
    }
    virtual ~Servlet()
    {
    }
    virtual int32_t handle(HttpRequest::ptr request,
                           HttpResponse::ptr response,
                           HttpSession::ptr session) = 0;

    const std::string &getName() const
    {
        return m_name;
    }

protected:
    std::string m_name;
};

class FunctionServlet : public Servlet
{
public:
    typedef std::shared_ptr<FunctionServlet> ptr;
    typedef std::function<int32_t(HttpRequest::ptr request,
                                  HttpResponse::ptr response,
                                  HttpSession::ptr session)>
        callback;

    FunctionServlet(callback cb);
    virtual int32_t handle(HttpRequest::ptr request,
                           HttpResponse::ptr response,
                           HttpSession::ptr session) override;

private:
    callback m_cb;
};

class ServletDispatch : public Servlet
{
public:
    typedef std::shared_ptr<ServletDispatch> ptr;
    typedef RWMutex RWMutexType;

    ServletDispatch();
    virtual int32_t handle(HttpRequest::ptr request,
                           HttpResponse::ptr response,
                           HttpSession::ptr session) override;

    void addServlet(const std::string &uri, Servlet::ptr slt);
    void addServlet(const std::string &uri, FunctionServlet::callback cb);
    void addGlobServlet(const std::string &uri, Servlet::ptr slt);
    void addGlobServlet(const std::string &uri, FunctionServlet::callback cb);

    void delServlet(const std::string &uri);
    void delGlobServlet(const std::string &uri);

    Servlet::ptr getDefault() const
    {
        return m_default;
    }
    void setDefault(Servlet::ptr v)
    {
        m_default = v;
    }

    Servlet::ptr getServlet(const std::string &uri);
    Servlet::ptr getGlobServlet(const std::string &uri);

    Servlet::ptr getMatchedServlet(const std::string &uri);

private:
    RWMutexType m_mutex;
    // uri(/sylar/xxx) -> servlet
    std::unordered_map<std::string, Servlet::ptr> m_datas;
    // uri(/sylar/*) -> servlet
    std::vector<std::pair<std::string, Servlet::ptr>> m_globs;
    // default servlet
    Servlet::ptr m_default;
};

class NotFoundServlet : public Servlet
{
public:
    typedef std::shared_ptr<NotFoundServlet> ptr;
    NotFoundServlet(const std::string &name);
    virtual int32_t handle(HttpRequest::ptr request,
                           HttpResponse::ptr response,
                           HttpSession::ptr session) override;

private:
    std::string m_name;
    std::string m_content;
};

}; // namespace http

} // namespace lunar

#endif