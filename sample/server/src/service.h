#ifndef __SERVICE_H__
#define __SERVICE_H__

class Service
{
public:
    static bool loadConfig(int argc, char *argv[]);

    //初始化守护进程
    static void initDaemon();
    //初始化普通进程
    static void initNodaemon();

private:
    static void startServer();

    static void catcher(int signo);
    static void trapSigint();
    //CTRL_C退出
    static void trapPrompt(int signo);
};

#endif  //__SERVICE_H__
