#include "mysql/mysql.h"
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

    private:
        MYSQL* m_conn;
    };
}