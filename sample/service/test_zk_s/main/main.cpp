#include <iostream>
#include "service.h"
#include "log/log.h"

using namespace std;


/**
 * 应用程序入口
 **/
int main(int argc, char* argv[])
{	
	//打开程序crash记录调用栈功能
    lspf::log::Log::EnableCrashRecord();

	//读入配置文件信息
	//Service::loadConfig(argc, argv);
	int port = atoi(argv[1]);
#ifdef DEBUG
    //以控制台程序的方式启动
	Service::initNodaemon();
#else
    //以后台服务的方式启动
	Service::initDaemon(argv[0]);
#endif  //DEBUG

    Service::startService(port);

	return 0;
}
