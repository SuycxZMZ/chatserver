#ifndef OFFLINEMSGMODEL_HPP
#define OFFLINEMSGMODEL_HPP

#include <string>
#include <vector>
using namespace std;

// 提供离线消息表的操作接口
class offlineMsgModel
{
public:
    // 存储用户离线消息
    void insert(int userid, string msg);

    // 删除用户离线消息
    void remove(int userid);

    // 查询用户离线消息
    vector<string> query(int userid);
};

#endif