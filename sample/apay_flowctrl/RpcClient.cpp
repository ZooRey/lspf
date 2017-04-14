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

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;


#include "gen-server/AccPayment.h"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;

using namespace  ::accpayment_thrift;


typedef boost::shared_ptr<TProtocol> TProtocol_Ptr;

int main() {
	boost::shared_ptr<TTransport> socket(new TSocket("127.0.0.1", 9090));
	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	TProtocol_Ptr protocol(new TBinaryProtocol(transport));
	AccPaymentClient client(protocol);

  try {
	transport->open();
	
	std::string request_;
	std::string return_;
	
	request_ = "{"version":"1.0.0","request_id":"254903","organize_code":"001440332334700","sign":"sc8M3qR+C7vWlonc0FPlj4JhSYG3hmtUtCI0NvxDrOi3S2H9ZA/uTi1D1Zj6gvbUFwykoOB66jIwzKltB0eTKo5bNLuZP7mMdH1b81kdDW1HSH9Eidy1SCHRuzNR6gyf0emWjOqdWsjGrDa5aTRkYT07dNN2VAaIFemwFdlrSG9CMhxo563j3/F6lwoHg4Cmem7737yPf+KM7mVWyh27eT4aodYL0tTUHufVzQeNulbRgD5ICsS135DQMmgEFIGKSvuSpO48Toa5geA/wpsafpVqmVwmEC1Z0YwUwOTmHwLCpiCFZcW8UyE4ojVV5wmp5DSgMuuPuEcHbOORbMND/w=="}";
	
	cout << "request_=" << request_ << endl;
	
	client.query(return_, request_);
	
	cout << "return_=" << return_ << endl;

  } catch (TTransportException& tex) {
    cout << "type:" << tex.getType() << "  ERROR: " << tex.what() << endl;
  } catch (TException& tx) {
    cout << "ERROR: " << tx.what() << endl;
  }
    if (!transport->isOpen())
    {
        cout << "transport->isNotOpen" << endl;
    }
    else
        cout << "transport->isOpen" << endl;

    transport->close();
}
