### README

编译 方式1：
```C++
cd build 
cmake ..
make
// 可执行文件放在 bin文件夹下
```
编译 方式2：
```C++
./build.sh
```

需要安装的环境 nginx mysql redis

致谢：

    第三方JSON库 https://github.com/nlohmann/json

    https://github.com/shenmingik

### 问题

1、登陆的时候服务器会在```_redis.subscribe(id);``` 这句话左右挂掉

gdb追踪是挂在```redis.cpp```的```void Redis::observer_channel_message()``` 函数中的处理redis返回语句。

解决：redis设有登陆用户和密码，服务端代码并没有该登陆过程导致登陆redis失败，最简单粗暴的方法是取消redis登陆密码```vim /etc/redis/redis.conf``` 注释掉 ```requirepass```这一句。
