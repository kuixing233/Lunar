#ifndef __LUNAR_RPC_RPC_SERVER_H__
#define __LUNAR_RPC_RPC_SERVER_H__

#include "../tcp_server.h"

namespace lunar
{

namespace rpc
{

class RpcServer : public TcpServer
{
public:
    typedef std::shared_ptr<RpcServer> ptr;

    RpcServer() : TcpServer()
    {
    }

    ~RpcServer()
    {
    }

    void setConnectionCallback(
        const std::function<void(const Socket::ptr &)> &cb)
    {
        m_connectionCallback = cb;
    }

    void setMessageCallback(const std::function<void(const Socket::ptr &)> &cb)
    {
        m_messageCallback = cb;
    }

    std::function<void(const Socket::ptr &)> &getConnectionCallback()
    {
        return m_connectionCallback;
    }

    std::function<void(const Socket::ptr &)> &getMessageCallback()
    {
        return m_messageCallback;
    }

protected:
    /**
     * @brief 处理新连接的Socket类
     */
    virtual void handleClient(Socket::ptr client);

private:
    std::function<void(const Socket::ptr &)> m_connectionCallback;
    std::function<void(const Socket::ptr &)> m_messageCallback;
};

} // namespace rpc

} // namespace lunar

#endif