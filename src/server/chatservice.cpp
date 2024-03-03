#include "chatservice.hpp"
#include "public.hpp"
#include <functional>
#include <string>
#include <vector>
#include <muduo/base/Logging.h>
using namespace std;
using namespace muduo;
// 获取单例对象的接口
ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

// 注册消息以及对应的回调 -- 初始化map
ChatService::ChatService()
{
    _msgHandlerMap.emplace(LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3));
    _msgHandlerMap.emplace(LOGINOUT_MSG, std::bind(&ChatService::loginOut, this, _1, _2, _3));
    _msgHandlerMap.emplace(REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3));
    _msgHandlerMap.emplace(ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3));
    _msgHandlerMap.emplace(ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3));
    _msgHandlerMap.emplace(CREATE_GROUP_MSG, std::bind(&ChatService::createGroup, this, _1, _2, _3));
    _msgHandlerMap.emplace(ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3));
    _msgHandlerMap.emplace(GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3));
    //连接 redis 服务器
    if (_redis.connect())
    {
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::redisSubscribeMsgHandler, this, _1, _2));
    }
}

// 服务器异常，重置业务
void ChatService::reset()
{
    _usermodel.resetState();
}

// 根据消息类型获取回调函数
MsgHandler ChatService::getHandler(int msgid)
{
    if (_msgHandlerMap.find(msgid) == _msgHandlerMap.end())
    {
        return [=](const TcpConnectionPtr &, json &, Timestamp)
        {
            LOG_ERROR << "msgid : " << msgid << " invalid !!! ";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}
// -------------------------------------- 业务回调实现 -------------------------------------- //
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // LOG_INFO << " do login service ";
    int id = js["id"].get<int>();
    string pwd = js["password"];
    User user = _usermodel.query(id);
    if (user.getId() == id && user.getPwd() == pwd)
    {
        if (user.getState() == "online")
        {
            // 登陆失败
            json response;
            response["msgid"] = LOGIN_ACK;
            response["errno"] = 2;
            response["errmsg"] = "已经登陆，不允许重复登录";
            conn->send(response.dump());
        }
        else
        {
            {
                lock_guard<mutex> lock(_connMutex);
                // 登陆成功，记录用户连接，加锁插入
                _userConnMap.emplace(id, conn);
            }

            // 登陆成功之后 向redis订阅id
            _redis.subscribe(id);

            // 登陆成功， 用户信息 offline --> online
            user.setState("online");
            _usermodel.updateState(user);

            json response;
            response["msgid"] = LOGIN_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 给用户发送离线时接收的消息
            vector<string> offlineMsg = _offlineMsgModel.query(id);
            if (!offlineMsg.empty())
            {
                response["offlinemsg"] = offlineMsg;
                // 把该用户的离线消息删除
                _offlineMsgModel.remove(id);
            }

            // 查询用户好友信息并返回
            vector<User> friendVec = _friendmodel.query(id);
            if (!friendVec.empty())
            {
                vector<string> friendstr;
                for (User &user : friendVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    friendstr.emplace_back(js.dump());
                }
                response["friends"] = friendstr;
            }
            // 查询该用户的群组信息并返回
            vector<Group> group_vec = _groupmodel.queryGroups(id);
            if (!group_vec.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupV;
                for (Group &group : group_vec)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();

                    vector<string> userV;
                    for (GroupUser &user : group.getUsers())
                    {
                        json js;
                        js["id"] = user.getId();
                        js["name"] = user.getName();
                        js["state"] = user.getState();
                        js["role"] = user.getRole();
                        userV.emplace_back(js.dump());
                    }
                    grpjson["users"] = userV;

                    groupV.push_back(grpjson.dump());
                }
                response["groups"] = groupV;
            }
            conn->send(response.dump());
        }
    }
    else
    {
        // 登陆失败
        json response;
        response["msgid"] = LOGIN_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户不存在";
        conn->send(response.dump());
    }
}
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    // LOG_INFO << " do reg service ";
    string name = js["name"];
    string pwd = js["password"];
    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool ret = _usermodel.insert(user);

    if (ret)
    {
        // 注册成功 给响应
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        // 找到对应的用户id并保存
        for (auto it : _userConnMap)
        {
            if (it.second == conn)
            {
                user.setId(it.first);
                _userConnMap.erase(it.first);
                break;
            }
        }
    }

    // 取消 redis 订阅
    _redis.unsubscribe(user.getId());

    // 更新用户信息
    user.setState("offline");
    _usermodel.updateState(user);
}

void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["to"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // 服务器原封不动把消息转发给to连接
            it->second->send(js.dump());
            return;
        }
    }

    // 当前服务器不在线 查询 toid 是否在线 在线的话发送给 redis 对应通道
    User user = _usermodel.query(toid);
    if (user.getState() == "online")
    {
        _redis.publish(toid, js.dump());
        return;
    }

    // 不在线
    _offlineMsgModel.insert(toid, js.dump());
}

//  msgid id friendid
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    //
    _friendmodel.insert(userid, friendid);
}

// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];
    // 存储新创建的群组信息
    Group group(-1, name, desc);
    if (_groupmodel.createGroup(group))
    {
        _groupmodel.addGroup(userid, group.getId(), "creator");
    }
}
// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupmodel.addGroup(userid, groupid, "normal");
}

// 群聊天
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupmodel.queryGroupUsers(userid, groupid);
    lock_guard<mutex> lock(_connMutex);
    for (int id : useridVec)
    {
        auto it = _userConnMap.find(id);
        if (it != _userConnMap.end())
        {
            it->second->send(js.dump());
        }
        else
        {
            User user = _usermodel.query(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());
            }
            else
            {
                _offlineMsgModel.insert(id, js.dump());
            }
        }
    }
}

// 注销业务
void ChatService::loginOut(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if (it != _userConnMap.end())
        {
            _userConnMap.erase(it);
        }
    }

    // 在redis中取消订阅
    _redis.unsubscribe(userid);

    User user(userid, "", "", "offline");
    _usermodel.updateState(user);
}

//redis订阅消息触发的回调函数
void ChatService::redisSubscribeMsgHandler(int channel, string message)
{
    //用户在线
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(channel);
    if (it != _userConnMap.end())
    {
        it->second->send(message);
        return;
    }

    //转储离线
    _offlineMsgModel.insert(channel, message);
}