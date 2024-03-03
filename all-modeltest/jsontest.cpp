#include "../thirdparty/json.hpp"
using json = nlohmann::json;
#include<iostream>
#include<vector>
#include<map>
#include<string>
using namespace std;

/*
json序列化
*/
string test1()
{
    json js;
    js["id"] = {1,2,3,4,5};
    // 添加key-value
    js["name"] = "zhang san";
    // 添加对象
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["liu shuo"] = "hello china";
    // 上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello world"}, {"liu shuo", "hello china"}}; 
    
    return js.dump();
}

int main()
{
    string recv = test1();
    // 反序列化
    json jsbuf = json::parse(recv);
    cout << jsbuf["id"] << endl;
    cout << jsbuf["name"] << endl;
    cout << jsbuf["msg"] << endl;
    cout << jsbuf["msg"]["liu shuo"] << endl;

    return 0;
}