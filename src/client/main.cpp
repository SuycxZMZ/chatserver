#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <unordered_map>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

// 客户端用原生socket接口
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "group.hpp"
#include "user.hpp"
#include "public.hpp"

#define PASSWORD_LENGTH 50
#define NAME_LENGTH 50
#define BUFF_SIZE 1024

// 记录当前系统用户登录的用户信息
User g_currentUser;
// 记录当前登陆用户的好友列表
vector<User> g_currentUserFriendList;
// 记录当前登陆用户的群组列表信息
vector<Group> g_currentUserGroupList;
// 显示当前登陆成功用户的基本信息
void showCurrentUserData();

// 接收线程
void readTaskHandler(int client_fd);
// 获取系统时间
string getCurrentTime();
// 主聊天页面程序
void mainMenu(int client_fd);

// command handler
void help(int fd = 0, string str = "");
//
void chat(int, string);
//
void addFriend(int, string);
//
void createGroup(int, string);
//
void addGroup(int, string);
//
void groupChat(int, string);
//
void loginOut(int, string);

// 控制主菜单是否继续显示
bool g_is_menu_running = false;
// 系统支持的客户端命令列表
unordered_map<string, string> command_map = {
    {"help", "显示所有支持的命令,格式help"},
    {"chat", "一对一聊天,格式chat:friendid:message"},
    {"addfriend", "添加好友,格式addfriend:friendid"},
    {"creategroup", "创建群组,格式creategroup:groupname:groupdesc"},
    {"addgroup", "加入群组,格式addgroup:groupid"},
    {"groupchat", "群聊,格式groupchat:groupid:message"},
    {"loginout", "注销,格式loginout"}
    };

// 注册系统支持的客户端命令处理
unordered_map<string, function<void(int, string)>> command_handler_map = {
    {"help", help},
    {"chat", chat},
    {"addfriend", addFriend},
    {"creategroup", createGroup},
    {"addgroup", addGroup},
    {"groupchat", groupChat},
    {"loginout", loginOut}
    };

// main线程用来发送，子线程用来接收
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "command invalid example: ./exename ipAddress port" << endl;
        exit(-1);
    }

    // 解析IP地址和端口号
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // socket通信
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == client_fd)
    {
        cerr << "create socket error" << endl;
        exit(-1);
    }
    // 通信服务器地址
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);
    // 连接服务器
    if (-1 == connect(client_fd, (sockaddr *)&server, sizeof(sockaddr_in)))
    {
        cout << "connect error" << endl;
        close(client_fd);
        exit(-1);
    }

    // 主业务
    for (;;)
    {
        ios::sync_with_stdio(1);
        // 显示首页面菜单 登录，注册，退出
        cout << "**********welcome**********" << endl;
        cout << "           1.  login" << endl;
        cout << "           2.  register" << endl;
        cout << "           3.  quit " << endl;
        cout << "please input your choice:";
        int choice = 0;
        int temp;
        string strbuf;
        // cin >> choice;
        scanf("%d", &choice);
        // 处理读入缓冲区的回车
        // getline(cin, strbuf);
        cin.get();
        cout << "choice : " << choice << endl;
        switch (choice)
        {
            case 1:
            {
                int id = 0;
                char pwd[PASSWORD_LENGTH] = {0};
                cout << "please input id:";
                cin >> id;
                cin.get();

                cout << "please input password:";
                cin.getline(pwd, PASSWORD_LENGTH);

                json js;
                js["msgid"] = LOGIN_MSG;
                js["id"] = id;
                js["password"] = pwd;
                string request = js.dump();

                // 发送
                int len = send(client_fd, request.c_str(), strlen(request.c_str()) + 1, 0);
                if (len == -1)
                {
                    cerr << "send reg msg error" << endl;
                }
                else
                {
                    // cout << "----------------- loginMsg send over ----------------" << endl;
                    // 接收服务器端反馈
                    char buffer[BUFF_SIZE] = {0};
                    len = recv(client_fd, buffer, BUFF_SIZE, 0);
                    if (len == -1)
                    {
                        cerr << "recv login response error" << endl;
                    }
                    else
                    {
                        // 反序列化
                        // cout << "----------------- start pase json ----------------" << endl;
                        json response_js = json::parse(buffer);
                        // cout << "----------------- pase json over ----------------" << endl;
                        // 登录失败
                        if (response_js["errno"].get<int>() != 0)
                        {
                            cerr << response_js["errmsg"] << endl;
                        }
                        else
                        {
                            // 登陆成功记录当前用户信息、好友信息、群组信息、离线消息
                            // 记录当前用户信息
                            g_currentUser.setId(response_js["id"].get<int>());
                            g_currentUser.setName(response_js["name"]);

                            // 记录当前用户的好友信息
                            if (response_js.contains("friends"))
                            {
                                vector<string> vec = response_js["friends"];
                                for (string &str : vec)
                                {
                                    json friend_js = json::parse(str);
                                    User user;
                                    user.setId(friend_js["id"].get<int>());
                                    user.setName(friend_js["name"]);
                                    user.setState(friend_js["state"]);
                                    g_currentUserFriendList.push_back(user);
                                }
                            }
                            // 群组信息
                            if (response_js.contains("groups"))
                            {
                                vector<string> vec = response_js["groups"];
                                for (string &str : vec)
                                {
                                    json group_js = json::parse(str);
                                    Group group;
                                    group.setId(group_js["id"].get<int>());
                                    group.setName(group_js["groupname"]);
                                    group.setDesc(group_js["groupdesc"]);

                                    vector<string> vec2 = group_js["users"];
                                    for (string &user_str : vec2)
                                    {
                                        GroupUser group_user;
                                        json group_user_js = json::parse(user_str);
                                        group_user.setId(group_user_js["id"].get<int>());
                                        group_user.setName(group_user_js["name"]);
                                        group_user.setState(group_user_js["state"]);
                                        group_user.setRole(group_user_js["role"]);

                                        group.getUsers().push_back(group_user);
                                    }
                                    g_currentUserGroupList.push_back(group);
                                }
                            }

                            // 显示登陆用户的基本信息
                            showCurrentUserData();

                            // 显示用户的离线消息
                            if (response_js.contains("offlinemsg"))
                            {
                                vector<string> vec = response_js["offlinemsg"];
                                for (auto &str : vec)
                                {
                                    json js = json::parse(str);
                                    cout << js["time"].get<string>() << "[" << js["id"] << "]" << js["name"].get<string>() << " said: " << js["msg"].get<string>() << endl;
                                }
                            }

                            // 如果当前用户注销，下一个用户执行到此，不用另外再起
                            // 登陆成功 --> 起一个线程进行读消息
                            static int threadflag = 0;
                            if (threadflag == 0)
                            {
                                threadflag++;
                                thread read_task(readTaskHandler, client_fd);
                                read_task.detach();
                            }
                            // 进入主界面
                            g_is_menu_running = true;
                            mainMenu(client_fd);
                        }
                    }
                }
            }
            break;
            case 2:
            {
                // 注册
                char name[NAME_LENGTH] = {0};
                char pwd[PASSWORD_LENGTH] = {0};
                cout << "user name:" << endl;
                cin.getline(name, NAME_LENGTH);

                cout << "password:" << endl;
                cin.getline(pwd, PASSWORD_LENGTH);

                // 序列化
                json js;
                js["msgid"] = REG_MSG;
                js["name"] = name;
                js["password"] = pwd;
                string request = js.dump();

                // 发送注册消息
                int len = send(client_fd, request.c_str(), strlen(request.c_str()) + 1, 0);
                if (-1 == len)
                {
                    cerr << "send reg msg error!!!" << endl;
                }
                else
                {
                    // 接收服务端反馈
                    char buffer[BUFF_SIZE] = {0};
                    len = recv(client_fd, buffer, BUFF_SIZE, 0);
                    if (-1 == len)
                    {
                        cerr << "recv reg response error!!!" << endl;
                    }
                    else
                    {
                        // 反序列化
                        cout << "lens : " << len << " buffer : " << buffer << endl;
                        json response_js = json::parse(buffer);
                        if (0 != response_js["errno"].get<int>())
                        {
                            // 注册失败
                            cerr << name << "is already exist, register error!!!" << endl;
                        }
                        else
                        {
                            // 注册成功
                            cout << name << "register success, and userid : " << response_js["id"] << " ,please rember it!!!" << endl;
                        }
                    }
                }
            }
            break;
            case 3:
            {
                // 退出
                close(client_fd);
                exit(0);
            }
            default:
            {
                cerr << "invalid input !!!" << endl;
                break;
            }
        }
    }

    return 0;
}

// 显示当前登录成功用户的基本信息
void showCurrentUserData()
{
    cout << "--------------------login user--------------------" << endl;
    cout << "current login uer => id: " << g_currentUser.getId() << " name: " << g_currentUser.getName() << endl;
    cout << "-------------------friend  list-------------------" << endl;
    if (!g_currentUserFriendList.empty())
    {
        for (User &user : g_currentUserFriendList)
        {
            cout << user.getId() << " " << user.getName() << " " << user.getState() << endl;
        }
    }
    cout << "--------------------group list--------------------" << endl;
    if (!g_currentUserGroupList.empty())
    {
        for (Group &group : g_currentUserGroupList)
        {
            cout << group.getId() << " " << group.getName() << " " << group.getDesc() << endl;

            // 打印群员信息
            cout << "========group user========" << endl;
            for (GroupUser &group_user : group.getUsers())
            {
                cout << group_user.getId() << " " << group_user.getName() << " " << group_user.getState() << " " << group_user.getRole() << endl;
            }
        }
    }
    cout << "--------------------------------------------------" << endl;
}

// 接收用户收到消息的线程
void readTaskHandler(int client_fd)
{
    for (;;)
    {
        char buffer[BUFF_SIZE] = {0};
        int len = recv(client_fd, buffer, BUFF_SIZE, 0);
        if (-1 == len || 0 == len)
        {
            close(client_fd);
            exit(-1);
        }

        json js = json::parse(buffer);
        // 一对一聊天
        if (js["msgid"].get<int>() == ONE_CHAT_MSG)
        {
            cout << js["time"].get<string>() << "[" << js["id"] << "]" << js["name"].get<string>() << " said: " << js["msg"].get<string>() << endl;
            continue;
        }
        // 群组聊天
        else if (js["msgid"].get<int>() == GROUP_CHAT_MSG)
        {
            cout << "group msg: [" << js["groupid"] << "]";
            cout << js["time"].get<string>() << "[" << js["id"] << "]" << js["name"].get<string>() << " said: " << js["msg"].get<string>() << endl;
            continue;
        }
    }
}

string getCurrentTime()
{
    auto tt = chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
            (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
            (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return string(date);
}

void mainMenu(int client_fd)
{
    help();
    char buffer[BUFF_SIZE] = {0};
    while (g_is_menu_running)
    {
        cin.getline(buffer, BUFF_SIZE);
        string command_buf(buffer);
        // 存储命令
        string command;
        int idx = command_buf.find(":");
        if (-1 == idx)
        {
            // help or loginout
            command = command_buf;
        }
        else
        {
            command = command_buf.substr(0, idx);
        }
        auto it = command_handler_map.find(command);
        if (it == command_handler_map.end())
        {
            cerr << "invalid input command!!!" << endl;
            continue;
        }
        it->second(client_fd, command_buf.substr(idx + 1, command_buf.size() - idx));
    }
}

// 打印系统支持的所有命令
void help(int, string)
{
    cout << "--------command list--------" << endl;
    for (auto &it : command_map)
    {
        cout << it.first << " : " << it.second << endl;
    }
    cout << endl;
}

// 一对一聊天
void chat(int clientfd, string str)
{
    int index = str.find(":");
    if (index == -1)
    {
        cerr << "chat command invaild" << endl;
    }

    int friend_id = atoi(str.substr(0, index).c_str());
    string message = str.substr(index + 1, str.size() - index);

    json js;
    js["msgid"] = ONE_CHAT_MSG;
    js["id"] = g_currentUser.getId();
    js["name"] = g_currentUser.getName();
    js["to"] = friend_id;
    js["msg"] = message;
    js["time"] = getCurrentTime();

    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send chat msg error" << endl;
    }
}

// 添加好友
void addFriend(int clientfd, string str)
{
    int friend_id = atoi(str.c_str());

    json js;
    js["msgid"] = ADD_FRIEND_MSG;
    js["id"] = g_currentUser.getId();
    js["friendid"] = friend_id;

    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send addfriend msg error" << endl;
    }
}

// 创建群聊
void createGroup(int clientfd, string str)
{
    int index = str.find(":");
    if (index == -1)
    {
        cerr << "creategroup command invalid!" << endl;
        return;
    }

    string group_name = str.substr(0, index);
    string group_desc = str.substr(index + 1, str.size() - index);

    json js;
    js["msgid"] = CREATE_GROUP_MSG;
    js["id"] = g_currentUser.getId();
    js["groupname"] = group_name;
    js["groupdesc"] = group_desc;

    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send creategroup msg error" << endl;
    }
}

// 加入群聊
void addGroup(int clientfd, string str)
{
    int group_id = atoi(str.c_str());

    json js;
    js["msgid"] = ADD_GROUP_MSG;
    js["id"] = g_currentUser.getId();
    js["groupid"] = group_id;

    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send addgroup msg error" << endl;
    }
}

// 群聊消息
void groupChat(int clientfd, string str)
{
    int index = str.find(":");
    if (index == -1)
    {
        cerr << "creategroup command invalid!" << endl;
        return;
    }

    int group_id = atoi(str.substr(0, index).c_str());
    string message = str.substr(index + 1, str.size() - index);

    json js;
    js["msgid"] = GROUP_CHAT_MSG;
    js["id"] = g_currentUser.getId();
    js["name"] = g_currentUser.getName();
    js["groupid"] = group_id;
    js["msg"] = message;
    js["time"] = getCurrentTime();

    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send grouochat msg error" << endl;
    }
}

// 注销
void loginOut(int clientfd, string)
{
    json js;
    js["msgid"] = LOGINOUT_MSG;
    js["id"] = g_currentUser.getId();
    string buffer = js.dump();

    string request = js.dump();

    int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send grouochat msg error" << endl;
    }
    else
    {
        g_is_menu_running = false;
        g_currentUserFriendList.clear();
        g_currentUserGroupList.clear();
    }
}