#include "rpc_server.h"
#include "../lunar.h"

namespace lunar
{

static lunar::Logger::ptr g_rpclogger = LUNAR_LOG_NAME("system");

namespace rpc
{

void RpcServer::handleClient(Socket::ptr client)
{

    LUNAR_LOG_INFO(g_rpclogger) << "RPC handleClient: " << *client;

    auto self = std::static_pointer_cast<RpcServer>(shared_from_this());

    m_ioWorker->addEvent(
        client->getSocket(),
        IOManager::READ,
        [self, client]() { self->getMessageCallback()(client); });

    // m_ioWorker->addEvent(client->getSocket(), IOManager::WRITE, [&self, client]() {
    //     client->send("hello", 5);
    // });
}

} // namespace rpc

} // namespace lunar