#include "../lunar/lunar.h"
#include "../lunar/objectPool.h"

lunar::Logger::ptr logger = LUNAR_LOG_ROOT();

class MyObject
{
public:
    typedef std::shared_ptr<MyObject> ptr;
    MyObject()
    {
        LUNAR_LOG_INFO(logger) << "构造 myObject";
    }

    ~MyObject()
    {
        LUNAR_LOG_INFO(logger) << "析构 myObject";
    }

    int value = 0;
};

void test(lunar::ObjectPool<MyObject> &pool)
{
    auto obj1 = pool.get();
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();
    auto obj2 = pool.get();
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();
    auto obj3 = pool.get();
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();
    auto obj4 = pool.get();
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();
    auto obj5 = pool.get();
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();
    auto obj6 = pool.get();
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();

    LUNAR_LOG_INFO(logger) << "============================================";

    pool.put(obj1);
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();
    pool.put(obj2);
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();
    pool.put(obj3);
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();
    pool.put(obj4);
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();
    pool.put(obj5);
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();
    pool.put(obj6);
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();

    LUNAR_LOG_INFO(logger) << "============================================";
    auto obj7 = pool.get();
    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();

    LUNAR_LOG_INFO(logger) << "============================================";

    MyObject::ptr obj(new MyObject);
    pool.put(obj);

    LUNAR_LOG_INFO(logger) << "============================================";

    pool.close();

    LUNAR_LOG_INFO(logger) << "pool's size = " << pool.get_size()
                           << " maxSize = " << pool.get_maxSize();
}

int main(int argc, char **argv)
{

    lunar::ObjectPool<MyObject> pool(5);

    lunar::IOManager iom(2);


    iom.schedule([&pool]() { test(pool); });

    return 0;
}