#ifndef __MY_SERVER_H__
#define __MY_SERVER_H__

class MyServer
{
public:
    static bool init(int port);
    //�����Զ������
    static bool start(int port);

    //ֹͣ�Զ������
    static void stop();
};

#endif // __MY_SERVER_H__
