/*
 * Tencent is pleased to support the open source community by making Pebble available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
 * Licensed under the MIT License (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <boost/thread/thread.hpp>
#include "message/message.h"
#include "message/zmq_driver.h"

using namespace std;
using namespace lspf::net;

int on_message(int handle, const char *msg, size_t msg_len, boost::shared_ptr<MessageReply> addr) {
    string str(msg, msg_len);
    printf("client handle:%d, msg:%s, len:%d\n", handle,
            str.c_str(), static_cast<int>(msg_len));
    return 0;
}

#define CHECK(e) \
    if ((e) != 0) {\
        printf("file:%s, line:%d, error:%s\n", __FILE__, __LINE__, Message::LastErrorMessage());\
        exit(1);\
    }

int main(int argc, char **argv) {

    ZMQDriver driver("tcp://127.0.0.1:8260");

    driver.Init(MessageDriver::MESSAGE_CLIENT);

    Message::RegisterDriver(&driver);

    driver.SetOnMessage(on_message);

    int handle = Message::NewHandle();

    CHECK(Message::Run(handle, &driver));

    printf("handle:%d\n", handle);

    const char * msg1 = "tcp 4";
    const char * msg2 = "tcp 5";

    for (int i=0; i<100000; i++){
        printf("count=%d\n", i);
        CHECK(Message::Send(handle, msg1, strlen(msg1)));
        //CHECK(Message::Send(handle, msg2, strlen(msg2)));
        usleep(5);
    }

    std::cout << "Error: " << Message::LastErrorMessage() <<std::endl;

    // Wait to exit.
    std::cout << "Press 'q' to quit" << std::endl;
    while( std::cin.get() != 'q') {}

    return 0;
}

