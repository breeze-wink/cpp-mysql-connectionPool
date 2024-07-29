#include "connection.h"
#include <mysql/mysql.h>
#include <iostream>

using namespace breeze::mysql;

Connection::Connection()
{
    m_conn = mysql_init(nullptr);
}
Connection::~Connection()
{
    if (m_conn != nullptr)
    {
        mysql_close(m_conn);
    }
}

bool Connection::connect(string ip, uint16_t port, string user, string password, string database)
{
    MYSQL* p = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), 
    password.c_str(), database.c_str(), port, nullptr, 0);

    if (p == nullptr)
    {
        std::cerr << "Failed to connect to database" << std::endl;
    }
    return p != nullptr;
}

bool Connection::update(string sql)
{
    //insert、 delete、 update
    if (mysql_query(m_conn, sql.c_str())) //0 success
    {
        std::cerr << "Failed to execute sql: " << sql << std::endl;
        return false;
    }

    return true;
}

MYSQL_RES* Connection::query(string sql)
{
    if (mysql_query(m_conn, sql.c_str()))
    {
        std::cerr << "Failed to execute sql: " << sql << std::endl;
        return nullptr;
    }
    
    return mysql_use_result(m_conn);
}
