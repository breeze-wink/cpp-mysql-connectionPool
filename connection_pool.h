#pragma once
#include "connection.h"
#include "singleton.h"
#include <atomic>
#include <memory>
#include <queue>
#include <condition_variable>
#include <mutex>

using namespace breeze::utility;

namespace breeze::mysql
{
    class ConnectionPool
    {
        SINGLETON(ConnectionPool);
    public:
        void init(const string& filename);
        std::shared_ptr<Connection> getConnection();

    private:
        void load_config(const string& filename);
        void scannerConnectionTask();
        void produceConnectionTask();
    private:
        string m_ip;
        uint16_t m_port;
        string m_user;
        string m_password;
        string m_database;
        int m_initSize;
        int m_maxSize;
        int m_maxIdleTime;
        int m_connectionTimeout;

        std::queue<Connection*> m_connectionQueue;
        std::mutex m_mutex;
        std::atomic_int m_connectionCount;
        std::condition_variable m_cv;
    };
}