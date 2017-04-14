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
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PlatformThreadFactory.h>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/TToString.h>


#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <iostream>
#include <stdexcept>
#include <sstream>

#include "gen-cpp/Demo.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

using namespace demo_thrift;

class DemoHandler : public DemoIf {
public:
  DemoHandler() {}

  void ping() { cout << "ping()" << endl; }

  int32_t add(const int32_t n1, const int32_t n2) {
    cout << "add(" << n1 << ", " << n2 << ")" << endl;
    return n1 + n2;
  }

  int32_t calculate(const int32_t logid, const Work& work) {
    cout << "calculate(" << logid << ", " << work << ")" << endl;
    int32_t val;

    switch (work.op) {
    case Operation::ADD:
      val = work.num1 + work.num2;
      break;
    case Operation::SUBTRACT:
      val = work.num1 - work.num2;
      break;
    case Operation::MULTIPLY:
      val = work.num1 * work.num2;
      break;
    case Operation::DIVIDE:
      if (work.num2 == 0) {
        InvalidOperation io;
        io.whatOp = work.op;
        io.why = "Cannot divide by 0";
        throw io;
      }
      val = work.num1 / work.num2;
      break;
    default:
      InvalidOperation io;
      io.whatOp = work.op;
      io.why = "Invalid Operation";
      throw io;
    }

    //SharedStruct ss;
    //ss.key = logid;
    //ss.value = to_string(val);

    //log[logid] = ss;

    return val;
  }
/*
  void getStruct(SharedStruct& ret, const int32_t logid) {
    cout << "getStruct(" << logid << ")" << endl;
    ret = log[logid];
  }
*/
  void zip() {
      cout << "zip()" << endl;
   }

  void query(std::string& _return, const std::string& request) {
    // Your implementation goes here
    printf("query\n");
	printf("request=%s\n", request.c_str());

	_return = "World";
  }

protected:
  //map<int32_t, SharedStruct> log;
};

/*
  DemoCloneFactory is code generated.
  DemoCloneFactory is useful for getting access to the server side of the
  transport.  It is also useful for making per-connection state.  Without this
  CloneFactory, all connections will end up sharing the same handler instance.
*/
class DemoCloneFactory : virtual public DemoIfFactory {
 public:
  virtual ~DemoCloneFactory() {}
  virtual DemoIf* getHandler(const ::apache::thrift::TConnectionInfo& connInfo)
  {
    boost::shared_ptr<TSocket> sock = boost::dynamic_pointer_cast<TSocket>(connInfo.transport);
    std::cout << "Incoming connection\n";
    std::cout << "\tSocketInfo: "  << sock->getSocketInfo() << "\n";
    std::cout << "\tPeerHost: "    << sock->getPeerHost() << "\n";
    std::cout << "\tPeerAddress: " << sock->getPeerAddress() << "\n";
    std::cout << "\tPeerPort: "    << sock->getPeerPort() << "\n";
    return new DemoHandler;
  }

  virtual void releaseHandler(DemoIf* handler){//( ::shared::SharedServiceIf* handler) {
    //delete handler;
  }

};

int main(int argc, char *argv[]) {
    int port = 9090;
    if (argc > 1){
        port = atoi(argv[1]);
    }

    boost::shared_ptr<DemoHandler> handler(new DemoHandler());
    boost::shared_ptr<TProcessor> processor(new DemoProcessor(handler));
    //shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
    //shared_ptr<TTransportFactory> transportFactory(new TBufferedTransportFactory());
    boost::shared_ptr<TProtocolFactory> protocol_factory(new TBinaryProtocolFactory());

    int server_threads = 100;
    boost::shared_ptr<ThreadManager> thread_manager = ThreadManager::newSimpleThreadManager(server_threads);//指定10个线程数
    boost::shared_ptr<PosixThreadFactory> threadFactory = boost::shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
    thread_manager->threadFactory(threadFactory);
    thread_manager->start();
    //TThreadPoolServer server(processor,serverTransport,transportFactory,protocol_factory,thread_manager);//使用线程池
    TNonblockingServer server(processor,protocol_factory, port, thread_manager);//使用线程池
    //TNonblockingServer server(processor,protocol_factory,port);//不使用线程池
    // throttle concurrent connections
    unsigned long mconn = 1024;
    if (mconn > 0) {
        printf("Throttle max_conn to %lu\n", mconn);
        server.setMaxConnections(mconn);
        server.setOverloadAction(T_OVERLOAD_CLOSE_ON_ACCEPT);
    }

    server.serve();

    // this function never returns
  return 0;
}
