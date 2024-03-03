#ifndef USERMODEL_HPP
#define USERMODEL_HPP
#include "user.hpp"
#include <string>
using namespace std;

class UserModel
{
public:
    // User表的增加方法
    bool insert(User& user);

    // 根据用户号码查询用户信息
    User query(int id);

    // 更新用户信息
    bool updateState(User user);

    // 重置用户的状态信息
    void resetState();
private:

};

#endif