#include "connection.h"
#include "connection_pool.h"
#include <chrono>
#include <thread>
#include <iostream>

#define USE_POOL 1
#define MULTI_THREAD 1
using namespace breeze::mysql;

constexpr int loop_times = 2000;
int main()
{
    auto begin = std::chrono::high_resolution_clock::now();


#if MULTI_THREAD
    #if USE_POOL
        auto conn_pool = Singleton<ConnectionPool>::Instance();

        conn_pool -> init("mysql.ini");

        auto test_thread = [&](){
            for (int i = 0; i < loop_times; ++ i)
            {
                auto conn = conn_pool -> getConnection();
                char sql[1024] = {0};
                sprintf(sql, "insert into users(username, age, sex) values('%s', %d, '%s');"
                , "zhang san", 20, "male");
                conn -> update(sql);
            }
        };
    #else
        auto test_thread = [](){
            for (int i = 0; i < loop_times; ++ i)
            {
                Connection conn;
                char sql[1024] = {0};
                sprintf(sql, "insert into users(username, age, sex) values('%s', %d, '%s');"
                , "zhang san", 20, "male");

                conn.connect("127.0.0.1", 3306, "breeze", "password", "mydatabase");
                conn.update(sql);
            }
        };

    #endif    
    std::thread threads[5];
    for (int i = 0; i < 5; ++ i)
    {
        threads[i] = std::thread(test_thread);
    }
    for (int i = 0; i < 5; ++ i)
    {
        threads[i].join();
    }
    
    
#else

    #if USE_POOL
        auto conn_pool = Singleton<ConnectionPool>::Instance();

        conn_pool -> init("mysql.ini");

        for (int i = 0; i < loop_times; ++ i)
        {
            auto conn = conn_pool -> getConnection();
            char sql[1024] = {0};
            sprintf(sql, "insert into users(username, age, sex) values('%s', %d, '%s');"
            , "zhang san", 20, "male");
            conn -> update(sql);
        }
    #else
        for (int i = 0; i < loop_times; ++ i)
        {
            Connection conn;
            char sql[1024] = {0};
            sprintf(sql, "insert into users(username, age, sex) values('%s', %d, '%s');"
            , "zhang san", 20, "male");

            conn.connect("127.0.0.1", 3306, "breeze", "password", "mydatabase");
            conn.update(sql);
        }
    #endif

#endif
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);

    std::cout << duration.count() << "ms" << std::endl;
    return 0;
}