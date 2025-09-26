#include "../lunar/address.h"
#include "../lunar/log.h"

lunar::Logger::ptr g_logger = LUNAR_LOG_ROOT();

void test()
{
    std::vector<lunar::Address::ptr> addrs;

    LUNAR_LOG_INFO(g_logger) << "begin";
    // bool v = lunar::Address::Lookup(addrs, "localhost:3080");
    // bool v = lunar::Address::Lookup(addrs, "www.baidu.com", AF_INET);
    // bool v = lunar::Address::Lookup(addrs, "www.sylar.top", AF_INET);
    bool v = lunar::Address::Lookup(addrs, "www.mihoyo.com", AF_INET);
    LUNAR_LOG_INFO(g_logger) << "end";
    if (!v)
    {
        LUNAR_LOG_ERROR(g_logger) << "lookup fail";
        return;
    }

    for (size_t i = 0; i < addrs.size(); ++i)
    {
        LUNAR_LOG_INFO(g_logger) << i << " - " << addrs[i]->toString();
    }

    auto addr = lunar::Address::LookupAny("localhost:4080");
    if (addr)
    {
        LUNAR_LOG_INFO(g_logger) << *addr;
    }
    else
    {
        LUNAR_LOG_ERROR(g_logger) << "error";
    }
}

void test_iface()
{
    std::multimap<std::string, std::pair<lunar::Address::ptr, uint32_t>>
        results;

    bool v = lunar::Address::GetInterfaceAddresses(results);
    if (!v)
    {
        LUNAR_LOG_ERROR(g_logger) << "GetInterfaceAddresses fail";
        return;
    }

    for (auto &i : results)
    {
        LUNAR_LOG_INFO(g_logger)
            << i.first << " - " << i.second.first->toString() << " - "
            << i.second.second;
    }
}

void test_ipv4()
{
    // auto addr = lunar::IPAddress::Create("www.sylar.top");
    auto addr = lunar::IPAddress::Create("127.0.0.8");
    if (addr)
    {
        LUNAR_LOG_INFO(g_logger) << addr->toString();
    }
}

int main(int argc, char **argv)
{
    // test_ipv4();
    // test_iface();
    test();
    return 0;
}