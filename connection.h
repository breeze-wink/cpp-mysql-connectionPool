#pragma once
#include "mysql/mysql.h"
#include <ctime>
#include <string>
using std::string;

namespace breeze::mysql
{
    class Connection
    {
    public:
        Connection();
        ~Connection();
        
        bool connect(string ip, uint16_t port, string user, string password, string database);
        bool update(string sql);
        MYSQL_RES* query(string sql);
        void refreshAliveTime() { m_alivetime = clock(); }
        clock_t getAliveTime() {return clock() - m_alivetime; }
    private:
        MYSQL* m_conn;
        clock_t m_alivetime;
    };
}