#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"
#include <string>
#include <functional>
using namespace std;
using namespace placeholders;
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string &nameArg) 
                       : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册连接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));
    // 注册处理消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));
    // 设置线程数量
    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr& conn)
{
    if (!conn->connected())
    {
        // 用户断开连接
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}
void ChatServer::onMessage( const TcpConnectionPtr& conn,
                    Buffer* buf,
                    Timestamp time)
{
    string recvbuf = buf->retrieveAllAsString();
    // 收到消息反序列化
    json js = json::parse(recvbuf);
    // 业务层解耦合
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn, js, time);
}