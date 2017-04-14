#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include "common/string_utility.h"
#include "manage/zk_client.h"
#include <pthread.h>

using namespace std;

#include <boost/thread/thread.hpp>

void thread_run()
{
    std::string ip;
    int port;

    for (; ; ){
        ZkClient::Instance()->GetServerInfo("test", port, ip);
        std::cout << "ip=" << ip << "  port=" << port << std::endl;
        usleep(2);
    }
}

int main(int argc,char *argv[])
{
	ZkClient::Instance()->Init("172.20.11.100:2181,172.20.11.100:2182,172.20.11.100:2183", "test,ceshi", 3000);

	ZkClient::Instance()->ConnectZK();

    boost::thread_group group;
     for(int num=0;num<5;num++)
        group.create_thread(boost::bind(&thread_run));
    group.join_all();

	getchar();

	return 0;
}
