#include "../lunar/channel.h"
#include "../lunar/lunar.h"

static lunar::Logger::ptr g_logger = LUNAR_LOG_NAME("system");

// void sender(lunar::Chan<int>& ch) {
//     for (int i = 0; i < 3; ++i) {
//         std::string str;
//         std::cin >> str;
//         int tmp = std::stoi(str);
//         ch.send(tmp);
//         LUNAR_LOG_INFO(g_logger) << "Sent: " << tmp;
//     }
//     ch.close();
//     LUNAR_LOG_INFO(g_logger) << "block chan is closed";
// }

// void receiver1(lunar::Chan<int>& ch) {
//     while (true) {
//         auto result = ch.receive();
//         int value = result.first;
//         bool closed = result.second;
//         if (closed) {
//             LUNAR_LOG_INFO(g_logger) << "Channel is closed";
//             break;
//         }
//         LUNAR_LOG_INFO(g_logger) << "Receiver " << 1 << " received: " << value;
//     }
// }

// void receiver2(lunar::Chan<int>& ch) {
//     while (true) {
//         auto result = ch.receive();
//         int value = result.first;
//         bool closed = result.second;
//         if (closed) {
//             LUNAR_LOG_INFO(g_logger) << "Channel is closed";
//             break;
//         }
//         LUNAR_LOG_INFO(g_logger) << "Receiver " << 2 << " received: " << value;
//     }
// }

// void sender_unblock(lunar::Chan<int>& ch) {
//     for (int i = 0; i < 3; ++i) {
//         std::string str;
//         std::cin >> str;
//         int tmp = std::stoi(str);
//         bool success = ch.send(tmp);
//         if(success) {
//             LUNAR_LOG_INFO(g_logger) << "Sent: " << tmp;
//         } else {
//             LUNAR_LOG_INFO(g_logger) << "Channel is full";
//         }
//     }
//     ch.close();
//     LUNAR_LOG_INFO(g_logger) << "block chan is closed";
// }

// void receiver_unblock(lunar::Chan<int>& ch) {
//     while (true) {
//         auto result = ch.receive();
//         auto data = std::move(result.first);
//         bool closed = result.second;
//         if (closed) {
//             LUNAR_LOG_INFO(g_logger) << "Channel is closed";
//             break;
//         }
//         if (data) {
//             LUNAR_LOG_INFO(g_logger) << *data;
//         } else {
//             LUNAR_LOG_INFO(g_logger) << "Received nullptr";
//         }
//         sleep(1);
//     }
// }

void send_3(lunar::Chan<int> &ch_int, lunar::Chan<std::string> &ch_str)
{
    // std::string str;
    // std::cin >> str;
    // int tmp = std::stoi(str);
    // bool success = ch_int.send(tmp);

    // std::string str_2;
    // std::cin >> str_2;
    // success = ch_str.send(str_2);

    // std::string str_3;
    // std::cin >> str_3;
    // tmp = std::stoi(str_3);
    // success = ch_int.send(tmp);

    // ch_int.send(999);
    // ch_int.send(888);
    // ch_int.send(777);
    // ch_int.send(666);
    // ch_int.send(555);
    // ch_int.send(444);
    ch_str.send("hello");

    // ch_int.close();
    // ch_str.close();
}

void read_3(lunar::Chan<int>::ptr ch_int, lunar::Chan<std::string>::ptr ch_str)
{

    // lunar::FiberCondition cond;
    auto cond = std::make_shared<lunar::FiberCondition>();
    // lunar::FiberMutex mutex;
    auto mutex = std::make_shared<lunar::FiberMutex>();
    auto tmp_int = 0;
    std::string tmp_str;
    // std::pair<std::unique_ptr<int>, bool> result_int;
    // auto result_int = std::make_shared<std::pair<std::unique_ptr<int>, bool> >();
    auto result_int = std::make_shared<lunar::result_chan<int>>();
    // result_int->isClosed = false;
    LUNAR_LOG_INFO(g_logger)
        << "result_int's count is " << result_int.use_count();
    // std::pair<std::unique_ptr<std::string>, bool> result_str;
    // auto result_str = std::make_shared<std::pair<std::unique_ptr<std::string>, bool> >();
    auto result_str = std::make_shared<lunar::result_chan<std::string>>();
    // result_str->second = false;

    while (true)
    {
        if (!result_int->isClosed)
        {
            // lunar::IOManager::GetThis()->schedule([&ch_int, &tmp_int, cond, result_int, mutex]() {
            //     LUNAR_LOG_INFO(g_logger) << "start read ch_int====================================";
            //     LUNAR_LOG_INFO(g_logger) << "cond's count is " << cond.use_count();
            //     LUNAR_LOG_INFO(g_logger) << "result_int's count is " << result_int.use_count();
            //     auto result = ch_int.receive();
            //     LUNAR_LOG_INFO(g_logger) << "next to lock";
            //     LUNAR_LOG_INFO(g_logger) << "mutex's count is " << mutex.use_count();
            //     mutex->lock();
            //     *result_int = std::move(result);
            //     LUNAR_LOG_INFO(g_logger) << "result_int's count is " << result_int.use_count();
            //     mutex->unlock();
            //     LUNAR_LOG_INFO(g_logger) << "have read from ch_int";
            //     auto tmp = std::move(result_int->first);
            //     if(tmp == nullptr) {
            //         LUNAR_LOG_INFO(g_logger) << "read a nullptr int";
            //     }
            //     if(tmp != nullptr) {
            //         tmp_int = *tmp;
            //         LUNAR_LOG_INFO(g_logger) << "tmp_int = " << tmp_int;
            //     }
            //     LUNAR_LOG_INFO(g_logger) << "cond's count is " << cond.use_count();
            //     cond->notify_all();
            //     LUNAR_LOG_INFO(g_logger) << "cond's count is " << cond.use_count();
            // });
            lunar::select<int>(ch_int, cond, mutex, result_int, []() {
                std::cout << "have read from ch_int" << std::endl;
            });
        }

        if (!result_str->isClosed)
        {
            // lunar::IOManager::GetThis()->schedule([&ch_str, &tmp_str, cond, result_str, mutex]() {
            //     LUNAR_LOG_INFO(g_logger) << "start read ch_str====================================";
            //     LUNAR_LOG_INFO(g_logger) << "cond's count is " << cond.use_count();
            //     LUNAR_LOG_INFO(g_logger) << "result_str's count is " << result_str.use_count();
            //     auto result = ch_str.receive();
            //     LUNAR_LOG_INFO(g_logger) << "next to lock";
            //     LUNAR_LOG_INFO(g_logger) << "mutex's count is " << mutex.use_count();
            //     mutex->lock();
            //     *result_str = std::move(result);
            //     LUNAR_LOG_INFO(g_logger) << "result_str's count is " << result_str.use_count();
            //     mutex->unlock();
            //     LUNAR_LOG_INFO(g_logger) << "have read from ch_str";
            //     auto tmp = std::move(result_str->first);
            //     if(tmp == nullptr) {
            //         LUNAR_LOG_INFO(g_logger) << "read a nullptr str";
            //     }
            //     if(tmp != nullptr) {
            //         tmp_str = *tmp;
            //         LUNAR_LOG_INFO(g_logger) << "tmp_str = " << tmp_str;
            //     }
            //     LUNAR_LOG_INFO(g_logger) << "cond's count is " << cond.use_count();
            //     cond->notify_all();
            //     LUNAR_LOG_INFO(g_logger) << "cond's count is " << cond.use_count();
            // });
            lunar::select<std::string>(ch_str, cond, mutex, result_str, []() {
                std::cout << "have read from ch_str" << std::endl;
            });
        }

        LUNAR_LOG_INFO(g_logger) << "cond's count is " << cond.use_count();
        cond->wait(*mutex);
        LUNAR_LOG_INFO(g_logger) << "cond's count is " << cond.use_count();
        LUNAR_LOG_INFO(g_logger) << "已经从wait中出来了";
        if (result_int->isClosed && result_str->isClosed)
        {
            mutex->unlock();
            LUNAR_LOG_INFO(g_logger)
                << "result_str's count is " << result_str.use_count();
            LUNAR_LOG_INFO(g_logger)
                << "result_int's count is " << result_int.use_count();
            LUNAR_LOG_INFO(g_logger)
                << "mutex's count is " << mutex.use_count();
            LUNAR_LOG_INFO(g_logger) << "cond's count is " << cond.use_count();
            break;
        }
    }
    LUNAR_LOG_INFO(g_logger) << "read stop";
}

int main(int argc, char **argv)
{
    auto ch_int = std::make_shared<lunar::Chan<int>>(2);
    auto ch_str = std::make_shared<lunar::Chan<std::string>>(2);
    lunar::IOManager iom(3);

    ch_int->close();
    ch_str->close();

    // iom.schedule([&ch_int, &ch_str]() {
    //     send_3(ch_int, ch_str);
    // });

    // sleep(2);

    iom.schedule([&ch_int, &ch_str]() { read_3(ch_int, ch_str); });

    // sleep(2);

    // ch_int.close();
    // ch_str.close();

    return 0;
}
