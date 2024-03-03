#ifndef GROUPMODEL_HPP
#define GROUPMODEL_HPP

#include "group.hpp"
#include <string>
#include <vector>
using namespace std;

class GroupModel
{
public:
    // 创建群组
    bool createGroup(Group &group);
    // 加入群组
    void addGroup(int userid, int groupid, string role);
    // 查询用户所在群组信息
    vector<Group> queryGroups(int userid);
    // 根据指定的 groupid查询用户id列表，除userid自己，用于群聊业务给其他成员发消息
    // chatservice 里面存的有每一个userid对应的连接
    vector<int> queryGroupUsers(int userid, int groupid);
};

#endif