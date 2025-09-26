#include "../../../lunar/lunar.h"
#include "../../../lunar/rpc/rpc_channelimpl.h"
#include "../../../lunar/rpc/rpc_controllerimpl.h"
#include "../user.pb.h"

lunar::Logger::ptr g_logger = LUNAR_LOG_ROOT();

void run()
{
    fixbug::UserServiceRpc_Stub stub(new lunar::rpc::RpcChannelImpl);
    fixbug::LoginRequest request;
    request.set_name("lunar");
    request.set_pwd("883721");
    fixbug::LoginResponse response;
    // RPC 同步调用
    lunar::rpc::RpcControllerImpl controller;
    for (int i = 0; i < 1000; ++i)
    {
        stub.Login(&controller, &request, &response, nullptr);

        if (controller.Failed())
        {
            LUNAR_LOG_ERROR(g_logger) << controller.ErrorText();
        }
        else
        {
            // RPC 调用完成 读取结果
            if (response.result().errcode() == 0)
            {
                LUNAR_LOG_INFO(g_logger)
                    << "rpc login response: " << response.success();
                std::string test_name_4 = response.test_name_4();
                std::string test_name_1 = response.test_name_1();
                LUNAR_LOG_INFO(g_logger) << "rpc test_name: " << test_name_1
                                         << " " << test_name_1.size();
                LUNAR_LOG_INFO(g_logger) << "rpc test_name: " << test_name_4
                                         << " " << test_name_4.size();
            }
            else
            {
                LUNAR_LOG_ERROR(g_logger)
                    << "failed to rpc login: " << response.result().errmsg();
            }
        }
    }
}

int main()
{
    lunar::IOManager iom(3);
    iom.schedule(run);
    // iom.stop();
    // run();
    return 0;
}