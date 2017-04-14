#include <iostream>
#include "service.h"
#include "log/log.h"

using namespace std;


/**
 * Ӧ�ó������
 **/
int main(int argc, char* argv[])
{
	//�򿪳���crash��¼����ջ����
    lspf::log::Log::EnableCrashRecord();

	//���������ļ���Ϣ
	Service::loadConfig(argc, argv);

#ifdef DEBUG
    //�Կ���̨����ķ�ʽ����
	Service::initNodaemon();
#else
    //�Ժ�̨����ķ�ʽ����
	Service::initDaemon(argv[0]);
#endif  //DEBUG

    Service::startService();

	return 0;
}
