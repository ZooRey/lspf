#ifndef __MY_SERVER_H__
#define __MY_SERVER_H__

class MyServer
{
public:
    static bool init(int port);
    //启动自定义服务
    static bool start(int port);

    //停止自定义服务
    static void stop();
};

#endif // __MY_SERVER_H__
