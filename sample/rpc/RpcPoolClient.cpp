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

#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/progress.hpp>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include "gen-cpp/Demo.h"

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace demo_thrift;

#include "mexception.h"
#include "rpc/connection_provider.h"
#include "rpc/connection_pool_mgr.h"

using namespace lspf::rpc;

typedef boost::shared_ptr<TProtocol> TProtocol_Ptr;

void rpcCall(){

    try {
        ConnectionProvider connectionProvider_("testrpc");

        boost::shared_ptr<TTransport> socket = connectionProvider_.GetTransport();
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        TProtocol_Ptr protocol(new TBinaryProtocol(transport));
        DemoClient client(protocol);

        cout << "1 + 1 = " << client.add(1, 1) << endl;

    } catch (MException& tx) {
        cout << "initPool: " << tx.what() << endl;
    } catch (TException& tx) {
        cout << "ERROR: " << tx.what() << endl;
    }

}

void thread_run(){
    for (int i=0; i<10000; i++)
    {
        rpcCall();
        cout << "rpcCall finish" << endl;
    }
}

bool initPool()
{
    try{
        ConnectionPoolMgr *mgr = ConnectionPoolMgr::Instance();

        mgr->CreateConnectionPool(ConnectionPoolMgr::TSOCKET, "testrpc", "127.0.0.1", 9090, 2, 10, 30);

    }catch (MException& tx) {
        cout << "initPool: " << tx.what() << endl;
        return false;
    }

    cout << "CreateConnectionPool success: " << endl;

    return true;
}

int main() {

    if (!initPool())
    {
        exit(-1);
    }

    sleep(5);

    boost::progress_timer t;

	boost::thread_group group;

    for(int num=0;num<5;num++){
        group.create_thread(boost::bind(&thread_run));
    }

    group.join_all();

    t.elapsed();

    sleep(31);

    ConnectionProvider connectionProvider_("testrpc");

    boost::shared_ptr<TTransport> socket = connectionProvider_.GetTransport();

    std::cout << "Press 'q' to quit" << std::endl;
    while( std::cin.get() != 'q') {}
    return 0;
}
