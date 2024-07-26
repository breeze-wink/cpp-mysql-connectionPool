#include "connection.h"
#include <cstdio>

using namespace breeze::mysql;

int main()
{
    Connection conn;
    bool flag = conn.connect("127.0.0.1", 3306, "breeze", "password", "mydatabase");

    if (!flag)
    {
        return -1;
    }

    string sql;
    sql.resize(128);
    sprintf(sql.data(), 
            "insert into users(username, age, sex) values('%s', '%d', '%s')",
            "zhang san", 18, "male"
    );

    conn.update(sql);

    return 0;
}