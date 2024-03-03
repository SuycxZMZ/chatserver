#include "friendmodel.hpp"
#include "db.hpp"

void friendModel::insert(int userid, int friendid)
{
    // 组装 sql 语句
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, '%d')",userid, friendid);
    
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

vector<User> friendModel::query(int userid)
{
    // 组装 sql 语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id  where b.userid=%d;", userid);
    
    MySQL mysql;
    vector<User> friendVec;
    if (mysql.connect())
    {
        MYSQL_RES* res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                friendVec.emplace_back(user);
            }
            mysql_free_result(res);
            return friendVec;
        }
    }
    return friendVec;
}