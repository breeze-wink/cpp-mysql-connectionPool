#include "connection_pool.h"
#include "connection.h"
#include "iniFile.h"
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

using namespace breeze::mysql;

void ConnectionPool::load_config(const string& filename)
{
    auto ini = IniFile(filename);
    m_ip = static_cast<string>(ini.get("mysql", "ip"));
    m_port = static_cast<int>(ini.get("mysql", "port"));
    m_user = static_cast<string>(ini.get("mysql", "username"));
    m_password = static_cast<string>(ini.get("mysql", "password"));
    m_database = static_cast<string>(ini.get("mysql", "database"));
    m_initSize = static_cast<int>(ini.get("mysql", "initSize"));
    m_maxSize = static_cast<int>(ini.get("mysql", "maxSize"));
    m_maxIdleTime = static_cast<int>(ini.get("mysql", "maxIdleTime"));
    m_connectionTimeout = static_cast<int>(ini.get("mysql", "connectionTimeout"));

}

void ConnectionPool::init(const string& filename)
{
    m_connectionCount = 0;
    load_config(filename);
    for (int i = 0; i < m_initSize; ++ i)
    {
        Connection* conn = new Connection();
        conn -> connect(m_ip, m_port, m_user, m_password, m_database);
        m_connectionQueue.push(conn);
        conn -> refreshAliveTime();
        m_connectionCount ++;
    }

    std::thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();
    std::thread scanner(&ConnectionPool::scannerConnectionTask, this);
    scanner.detach();
}

std::shared_ptr<Connection> ConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_connectionQueue.empty())
    {
        auto ret = m_cv.wait_for(lock, std::chrono::seconds(m_connectionTimeout));
        if (ret == std::cv_status::timeout && m_connectionQueue.empty())
        {
            return nullptr;
        }
    }

    //自定义shared_ptr的删除器，使用完归还到队列
    std::shared_ptr<Connection> sp(m_connectionQueue.front(),
        [&](Connection* conn){
            std::unique_lock<std::mutex> lock(m_mutex);
            m_connectionQueue.push(conn);
            conn -> refreshAliveTime();
        }
    );
    m_connectionQueue.pop();

    m_cv.notify_all();
    return sp;
    
}

void ConnectionPool::produceConnectionTask()
{
    while (true)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_connectionQueue.empty())
        {
            m_cv.wait(lock);
        }

        //允许继续创建新的连接
        if (m_connectionCount < m_maxSize)
        {
            Connection* conn = new Connection();
            conn -> connect(m_ip, m_port, m_user, m_password, m_database);
            m_connectionQueue.push(conn);
            conn -> refreshAliveTime();
            m_connectionCount ++;
        }

        m_cv.notify_all();
    }
}

void ConnectionPool::scannerConnectionTask()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(m_maxIdleTime));
        std::unique_lock<std::mutex> lock(m_mutex);

        while (m_connectionCount > m_initSize)
        {
            Connection* conn = m_connectionQueue.front();
            if (conn -> getAliveTime() > m_maxIdleTime * 1000)
            {
                m_connectionQueue.pop();
                m_connectionCount --;
                delete conn;
                conn = nullptr;
            }
            else
            {
                break;
            }
        }
    }
}