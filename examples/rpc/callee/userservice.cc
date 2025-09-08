#include "../user.pb.h"
#include "../../../lunar/rpc/rpc_provider.h"
#include "../../../lunar/lunar.h"

lunar::Logger::ptr g_logger = ALPHA_LOG_ROOT();

// 本地服务 提供了两个进程内的本地方法 Login 和 GetFriendList
class UserService : public fixbug::UserServiceRpc {
public:
    UserService()
        :fixbug::UserServiceRpc() {}
    bool Login(const std::string& name, const std::string& pwd) {
        ALPHA_LOG_INFO(g_logger) << "doing local sevice: Login";
        ALPHA_LOG_INFO(g_logger) << "name: " << name;
        ALPHA_LOG_INFO(g_logger) << "pwd: " << pwd;
        return true;
    }

    void Login(::google::protobuf::RpcController* controller
        ,const ::fixbug::LoginRequest* request
        ,::fixbug::LoginResponse* response
        ,::google::protobuf::Closure* done) override {

        // 框架给业务上报了请求参数LoginRequest 应用获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 本地业务
        bool res = Login(name, pwd);

        // 写入响应
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(res);
        response->set_test_name_1("lunar");
        response->set_test_name_4("fuck you");

        done->Run();
    }
};

void run() {
    // 把 USerService 对象发布到 RPC 节点上
    lunar::rpc::RpcProvider* provider = new lunar::rpc::RpcProvider;
    provider->notifyService(new UserService());

    provider->run();
}

int main() {
    lunar::IOManager iom(5);
    iom.schedule(run);
    // iom.stop();
    // run();
    return 0;
}