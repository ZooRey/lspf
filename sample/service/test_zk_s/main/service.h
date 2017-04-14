#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <string>

class Service
{
public:
    static bool loadConfig(int argc, char *argv[]);

    //��ʼ���ػ�����
    static void initDaemon(const std::string &app_name);
    //��ʼ����ͨ����
    static void initNodaemon();

    static void startService(int port);
private:
    static void startServer();

    static void catcher(int signo);
    static void trapSigint();
    //CTRL_C�˳�
    static void trapPrompt(int signo);
};

#endif  //__SERVICE_H__
