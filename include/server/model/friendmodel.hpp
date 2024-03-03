#ifndef FRIENDMODEL_HPP
#define FRIENDMODEL_HPP

#include <vector>
#include "user.hpp"
using namespace std;

class friendModel
{
public:
    // 添加好友
    void insert(int userid, int friendid);
    // 返回好友列表
    vector<User> query(int userid);
};

#endif