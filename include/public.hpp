#ifndef PUBLIC_HPP
#define PUBLIC_HPP

/*
* server client的公共文件 定义消息封包解包
*
*/  
enum EnMsgType
{
    // 登陆消息
    LOGIN_MSG = 1,
    LOGIN_ACK,
    // 注册消息
    REG_MSG,
    // 注册响应
    REG_MSG_ACK,
    // 聊天消息
    ONE_CHAT_MSG,
    // 添加好友消息
    ADD_FRIEND_MSG,
    // 创建群组
    CREATE_GROUP_MSG,
    // 加入群组
    ADD_GROUP_MSG,
    // 群聊天
    GROUP_CHAT_MSG,
    // 注销
    LOGINOUT_MSG
};


#endif