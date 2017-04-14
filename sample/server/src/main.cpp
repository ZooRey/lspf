#include <iostream>
#include "service.h"
#include "common/log.h"

using namespace std;


/**
 * Ӧ�ó������
 **/
int main(int argc, char* argv[])
{
	//�򿪳���crash��¼����ջ����
    Log::EnableCrashRecord();

	//���������ļ���Ϣ
	Service::loadConfig(argc, argv);

#ifdef DEBUG
    //�Կ���̨����ķ�ʽ����
	Service::initNodaemon();
#else
    //�Ժ�̨����ķ�ʽ����
	Service::initDaemon();
#endif  //DEBUG

	return 0;
}
