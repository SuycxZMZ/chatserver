#ifndef CHATSERVICE_HPP
#define CHATSERVICE_HPP

#include <unordered_map>
#include <muduo/net/TcpConnection.h>
#include <functional>
#include <mutex>

#include "json.hpp"
#include "usermodel.hpp"
#include "offlinemsgmodel.hpp"
#include "friendmodel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"
using namespace std;
using namespace muduo;
using namespace muduo::net;
using json = nlohmann::json;

using MsgHandler = function<void(const TcpConnectionPtr&, json&, Timestamp)>;

// 业务类 --- 单例模式，只需要一个
class ChatService
{
public:
    // 获取单例对象的接口
    static ChatService* instance();
    // 获取对应的回调
    MsgHandler getHandler(int msgid);
    // 回调
    void login(const TcpConnectionPtr& conn, json& js, Timestamp time);
    void reg(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 处理客户端异常断开连接
    void clientCloseException(const TcpConnectionPtr& conn);
    // 一对一聊天
    void oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 添加好友业务
    void addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 创建群组业务
    void createGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 加入群组业务
    void addGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 群聊天
    void groupChat(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 注销业务
    void loginOut(const TcpConnectionPtr& conn, json& js, Timestamp time);
    //redis订阅消息触发的回调函数
    void redisSubscribeMsgHandler(int channel, string message);
    // 服务器异常， 重置业务
    void reset();
private:
    ChatService();
    // 存储业务ID对应的处理回调
    unordered_map<int, MsgHandler> _msgHandlerMap;

    // 存储登录用户的连接
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    
    // 保护_userConnMap 的线程安全
    mutex _connMutex;

    // 数据操作类对象
    UserModel _usermodel;

    // 离线消息处理对象
    offlineMsgModel _offlineMsgModel;

    // 添加好友操作类
    friendModel _friendmodel;

    // 群聊操作类
    GroupModel _groupmodel;

    // redis 操作对象
    Redis _redis;
};

#endif