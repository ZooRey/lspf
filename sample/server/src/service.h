#ifndef __SERVICE_H__
#define __SERVICE_H__

class Service
{
public:
    static bool loadConfig(int argc, char *argv[]);

    //��ʼ���ػ�����
    static void initDaemon();
    //��ʼ����ͨ����
    static void initNodaemon();

private:
    static void startServer();

    static void catcher(int signo);
    static void trapSigint();
    //CTRL_C�˳�
    static void trapPrompt(int signo);
};

#endif  //__SERVICE_H__
