#include "connection_pool.h"
#include "connection.h"
#include "iniFile.h"
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
    m_initSize = static_cast<int>(ini.get("mysql", "initsize"));
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
        m_connectionCount ++;
    }
}