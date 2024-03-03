#include "offlinemsgmodel.hpp"
#include "db.hpp"

void offlineMsgModel::insert(int userid, string msg)
{
    // 组装 sql 语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d, '%s')",userid, msg.c_str());
    
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 删除用户离线消息
void offlineMsgModel::remove(int userid)
{
    // 组装 sql 语句
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid = %d", userid);
    
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查询用户离线消息
vector<string> offlineMsgModel::query(int userid)
{
    // 组装 sql 语句
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d",userid);
    
    MySQL mysql;
    vector<string> allOfflineMsg;
    if (mysql.connect())
    {
        MYSQL_RES* res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                allOfflineMsg.emplace_back(row[0]);
            }
            mysql_free_result(res);
            return allOfflineMsg;
        }
    }
    return allOfflineMsg;
}