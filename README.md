# Lunar 高性能分布式系统框架

## 项目简介

Lunar 是一个基于 C++11 开发的高性能分布式系统框架，采用协程调度、事件驱动的架构设计。框架提供了完整的网络编程、RPC 通信、HTTP 服务、数据库访问等功能模块，支持高并发、低延迟的分布式应用开发。

## 核心特性

- **协程调度系统**：基于 ucontext 实现的 M:N 协程调度器
- **高性能网络 I/O**：基于 epoll 的异步 I/O 事件循环
- **RPC 通信框架**：支持 Protocol Buffers 的分布式 RPC 调用
- **HTTP 服务器**：完整的 HTTP/1.1 协议支持和 Servlet 框架
- **通道通信机制**：类似 Go 语言的 Channel 通信模式
- **配置管理系统**：基于 YAML 的动态配置管理
- **日志系统**：多级别、多输出的异步日志框架
- **数据库支持**：MySQL、MongoDB 数据库访问接口

## 目录结构

```
Lunar/
├── lunar/                  # 核心框架代码
│   ├── *.h/*.cc           # 核心模块实现
│   ├── http/              # HTTP 服务模块
│   ├── rpc/               # RPC 通信模块
│   └── db/                # 数据库访问模块
├── examples/              # 示例代码
├── tests/                 # 单元测试
├── cmake/                 # CMake 构建脚本
└── bin/                   # 编译输出目录
```

## 核心模块详解

### 1. 协程调度系统 (Fiber & Scheduler)

**技术特点：**
- 基于 ucontext 实现用户态协程切换
- N:M 协程调度模型，支持多线程协程池
- 协程状态管理：INIT、HOLD、EXEC、TERM、READY、EXCEPT
- 支持协程间的同步原语：FiberMutex、FiberCondition

**核心文件：**
- `fiber.h/cc` - 协程实现
- `scheduler.h/cc` - 协程调度器
- `fiber_sync.h` - 协程同步原语

**应用场景：**
- 高并发网络服务
- 异步任务处理
- 协程池管理

### 2. 网络 I/O 系统 (IOManager)

**技术特点：**
- 基于 Linux epoll 的事件驱动模型
- 集成定时器管理 (TimerManager)
- 支持读写事件的异步处理
- 文件描述符上下文管理

**核心文件：**
- `iomanager.h/cc` - I/O 事件管理器
- `timer.h/cc` - 定时器管理
- `fd_manager.h/cc` - 文件描述符管理

**应用场景：**
- 网络服务器开发
- 异步 I/O 处理
- 定时任务调度

### 3. 网络通信模块 (Socket & Stream)

**技术特点：**
- 封装 TCP/UDP Socket 操作
- 支持 IPv4/IPv6 地址解析
- 流式数据处理接口
- SSL/TLS 加密通信支持

**核心文件：**
- `socket.h/cc` - Socket 封装
- `address.h/cc` - 网络地址处理
- `socket_stream.h/cc` - Socket 流接口
- `tcp_server.h/cc` - TCP 服务器基类

**应用场景：**
- TCP/UDP 网络编程
- 客户端/服务器通信
- 网络代理服务

### 4. RPC 通信框架

**技术特点：**
- 基于 Protocol Buffers 的序列化
- 支持同步/异步 RPC 调用
- 集成 ZooKeeper 服务发现
- 自定义 RPC 协议头

**核心文件：**
- `rpc/rpc_server.h/cc` - RPC 服务器
- `rpc/rpc_client.h/cc` - RPC 客户端
- `rpc/rpc_provider.h/cc` - RPC 服务提供者
- `rpc/rpc_channelimpl.h/cc` - RPC 通道实现
- `rpc/zkclient.h/cc` - ZooKeeper 客户端

**应用场景：**
- 分布式服务调用
- 微服务架构
- 服务注册与发现

### 5. HTTP 服务模块

**技术特点：**
- 完整的 HTTP/1.1 协议支持
- Servlet 风格的请求处理
- 支持 Keep-Alive 长连接
- HTTP 解析器基于 Ragel 状态机

**核心文件：**
- `http/http_server.h/cc` - HTTP 服务器
- `http/http_session.h/cc` - HTTP 会话管理
- `http/http.h/cc` - HTTP 协议实现
- `http/servlet.h/cc` - Servlet 框架
- `http/http_parser.h/cc` - HTTP 解析器

**应用场景：**
- Web 服务开发
- RESTful API 服务
- HTTP 代理服务

### 6. 通道通信系统 (Channel)

**技术特点：**
- 类似 Go 语言的 Channel 机制
- 支持阻塞/非阻塞模式
- 协程间安全的数据传递
- 支持 select 多路复用

**核心文件：**
- `channel.h` - 通道实现

**应用场景：**
- 协程间通信
- 生产者-消费者模式
- 事件传递机制

### 7. 配置管理系统 (Config)

**技术特点：**
- 基于 YAML 格式的配置文件
- 支持动态配置更新
- 类型安全的配置访问
- 配置变更监听机制

**核心文件：**
- `config.h/cc` - 配置管理器

**应用场景：**
- 应用配置管理
- 动态参数调整
- 环境配置切换

### 8. 日志系统 (Log)

**技术特点：**
- 多级别日志输出 (DEBUG/INFO/WARN/ERROR/FATAL)
- 支持控制台和文件输出
- 自定义日志格式
- 异步日志写入

**核心文件：**
- `log.h/cc` - 日志系统实现

**应用场景：**
- 应用日志记录
- 调试信息输出
- 系统监控

### 9. 数据库访问模块

**技术特点：**
- 统一的数据库访问接口
- 支持 MySQL 和 MongoDB
- 连接池管理
- 事务支持

**核心文件：**
- `db/db.h` - 数据库接口定义
- `db/mysql.h/cc` - MySQL 访问实现
- `db/mongo.h/cc` - MongoDB 访问实现

**应用场景：**
- 数据持久化
- 数据库操作封装
- ORM 框架基础

### 10. 内存管理与工具模块

**技术特点：**
- 字节数组管理 (ByteArray)
- 对象池模式 (ObjectPool)
- 线程安全的单例模式
- 系统工具函数集合

**核心文件：**
- `bytearray.h/cc` - 字节数组管理
- `objectPool.h` - 对象池实现
- `singleton.h` - 单例模式
- `util.h/cc` - 工具函数
- `mutex.h/cc` - 互斥锁实现
- `thread.h/cc` - 线程封装

## 快速开始

### 环境要求

- **操作系统**: Linux (推荐 Ubuntu 18.04+)
- **编译器**: GCC 7.0+ (支持 C++11)
- **依赖库**:
  - yaml-cpp
  - protobuf
  - zookeeper
  - openssl
  - mysql-client (可选)
  - mongodb-client (可选)

### 编译安装

1. **克隆项目**
   ```bash
   git clone https://github.com/LunarMinZ/Lunar.git
   cd Lunar
   ```

2. **安装依赖**
   ```bash
   # Ubuntu/Debian
   sudo apt-get update
   sudo apt-get install -y cmake build-essential
   sudo apt-get install -y libyaml-cpp-dev libprotobuf-dev
   sudo apt-get install -y libzookeeper-mt-dev libssl-dev
   sudo apt-get install -y libmysqlclient-dev libmongoc-dev
   ```

3. **编译项目**
   ```bash
   mkdir build && mkdir bin
   cd build
   cmake ..
   make -j$(nproc)
   ```

4. **运行测试**
   ```bash
   # 运行 RPC 测试
   ./bin/test_rpc_callee &  # 启动服务端
   ./bin/test_rpc_caller    # 启动客户端
   ```

### 简单示例

#### HTTP 服务器示例

```cpp
#include "lunar/lunar.h"
#include "lunar/http/http_server.h"

int main() {
    lunar::IOManager iom(2);
    lunar::http::HttpServer::ptr server(new lunar::http::HttpServer(true));
    lunar::Address::ptr addr = lunar::Address::LookupAnyIPAddress("0.0.0.0:8020");
    
    server->getServletDispatch()->addServlet("/hello", 
        [](lunar::http::HttpRequest::ptr req,
           lunar::http::HttpResponse::ptr rsp,
           lunar::http::HttpSession::ptr session) {
            rsp->setBody("Hello, Lunar!");
            return 0;
        });
    
    server->bind(addr);
    server->start();
    
    iom.stop();
    return 0;
}
```

#### RPC 服务示例

```cpp
#include "lunar/lunar.h"
#include "lunar/rpc/rpc_server.h"

class EchoService : public EchoServiceRpc {
public:
    void Echo(google::protobuf::RpcController* controller,
              const EchoRequest* request,
              EchoResponse* response,
              google::protobuf::Closure* done) override {
        response->set_message("Echo: " + request->message());
        done->Run();
    }
};

int main() {
    lunar::rpc::RpcServer server;
    server.registerService(new EchoService());
    server.start("127.0.0.1", 8000);
    return 0;
}
```

## 性能特性

- **高并发**: 支持数万级并发连接
- **低延迟**: 协程切换开销 < 100ns
- **内存效率**: 协程栈大小可配置 (默认 128KB)
- **网络性能**: 基于 epoll 的零拷贝 I/O
