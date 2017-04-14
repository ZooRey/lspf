#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "common/string_utility.h"
#include "manage/zk_server.h"
#include <pthread.h>

using namespace std;

int main(int argc,char *argv[])
{
    if (argc < 3){
        std::cout << "input param error" << std::endl;
        return -1;
    }

	ZkServer::Instance()->Init("172.20.11.100:2181,172.20.11.100:2182,172.20.11.100:2183", 3000);

    ZkServer::Instance()->SetServiceInfo(argv[1], argv[2]);

	ZkServer::Instance()->Publish();

	std::cout << "Register ..." << std::endl;
    //ZkServer::Instance()->Register();

    //std::cout << "Register end..." << std::endl;
	//ZkServer::Instance()->Run();

	getchar();

	return 0;
}
