#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include<termios.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <json/json.h>
#include "gen-server/CupsChannel.h"
#include "common/string_utility.h"
#include "zoo_keeper_client.h"
#include <pthread.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace std;
using namespace  ::payment_channel;

void* DoTrans(void* arg)
{
	while(1)
	{
		int port;
		string ip;
		
		if (ZooKeeperClient::Instance()->GetServerInfo(port, ip) != 0)
		{
			continue;
		}
		
		boost::shared_ptr<TTransport> sock = boost::make_shared<TSocket>(ip, port); 
		boost::shared_ptr<TTransport> transport = boost::make_shared<TBufferedTransport>(sock); 
		boost::shared_ptr<TProtocol> protocol = boost::make_shared<TBinaryProtocol>(transport); 
		CupsChannelClient client(protocol); 
		
		char tmp[5+1] = {0};
		sprintf(tmp, "%05d", port);
		string req, rsp;
		req = ip + ":" + string(tmp);
		try 
		{  
			cout << "client start..., ip=" << ip << ", port=" << port << endl;
			transport->open(); 
			//while(1)
			{			
				client.DoTrans(rsp, string(tmp), req);
				cout << "rsp:" << rsp << endl;
				//sleep(1);
			}
			transport->close(); 	  
		} catch (TException &tx) 
		{ 
			printf("ERROR: %s\n", tx.what()); 
		} 
	}
	
	return NULL;
}

int main(int argc,char *argv[])
{
	ZooKeeperClient::Instance()->Init(3000, "test_zk_s", "127.0.0.1:2181,127.0.0.1:2182,127.0.0.1:2183");
	if (ZooKeeperClient::Instance()->Connect() != 0)
	{
		return -1;
	}
	if (ZooKeeperClient::Instance()->GetChildren() != 0)
	{
		return -1;
	}
	
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, DoTrans, NULL);
	
	getchar();

	
	return 0;
}