/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <stdio.h>
#include <getopt.h>
#include <event.h>
#include <string>
#include <list>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/thread.hpp>

#include <thrift/async/TAsyncChannel.h>
#include <thrift/async/TEvhttpClientChannel.h>
#include <thrift/async/TAsyncProtocolProcessor.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "gen-cpp/Calculator.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::async;
using namespace threads_thrift;


typedef boost::shared_ptr<TProtocol> TProtocol_Ptr;
	
void thread_run()
{
  boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
  boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
  TProtocol_Ptr protocol(new TBinaryProtocol(transport));	
	CalculatorClient client(protocol);

/*	
	socket->setConnTimeout(60);
	socket->setSendTimeout(60);
	socket->setRecvTimeout(60);
*/	
  try {
	transport->open();
    client.ping();
    cout << "ping()" << endl;

    cout << "1 + 1 = " << client.add(1, 1) << endl;

    Work work;
    work.op = Operation::DIVIDE;
    work.num1 = 1;
    work.num2 = 0;

    try {
      client.calculate(1, work);
      cout << "Whoa? We can divide by zero!" << endl;
    } catch (InvalidOperation& io) {
      cout << "InvalidOperation: " << io.why << endl;
      // or using generated operator<<: cout << io << endl;
      // or by using std::exception native method what(): cout << io.what() << endl;
    }

    work.op = Operation::SUBTRACT;
    work.num1 = 15;
    work.num2 = 10;
    int32_t diff = client.calculate(1, work);
    cout << "15 - 10 = " << diff << endl;
    
    transport->close();
  } catch (TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }

}

void myCalculator(CalculatorCobClient* client)
{
    int32_t res;
    res  = client->recv_add();
    printf("Calculator recv:%d\n", res);
}

event_base* evbase = event_base_new();	

void cob_thread_run(int num)
{
	try{		
		boost::shared_ptr<TAsyncChannel> channel(new TEvhttpClientChannel("127.0.0.1", "/", "127.0.0.1", 9091, evbase));
		boost::shared_ptr<CalculatorCobClient> client(new CalculatorCobClient(channel, new TBinaryProtocolFactory()));
		//CalculatorCobClient client(channel, new TBinaryProtocolFactory());
		boost::function<void(CalculatorCobClient* client)> cob = boost::bind(&myCalculator, _1);
		
		client->add(cob, 1, 2);
		cout << "add1" << endl;
		client->add(cob, 3, 4);
		cout << "add1" << endl;
		client->add(cob, 5, 6);
		cout << "add1" << endl;
		client->add(cob, 7, 8);
		cout << "add1" << endl;
		//sleep(2);
		event_base_dispatch(evbase);
		event_base_free(evbase);
	}
	catch(...)
	{
		printf("exception");
		return ;
    }

    return;
}


int main() {
	 
	boost::thread_group group;	
  try {
 
    for(int num=0;num<1;num++)
		group.create_thread(boost::bind(&cob_thread_run, num));
	group.join_all();

    
  } catch (TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
  
  return 0;
}
