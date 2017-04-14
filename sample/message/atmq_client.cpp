#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <boost/thread/thread.hpp>
#include "message/message.h"
#include "message/atmq_driver.h"

using namespace std;
using namespace lspf::net;

int on_message(int handle, const char *msg, size_t msg_len, boost::shared_ptr<MessageReply>) {
    string str(msg, msg_len);
    printf("client handle:%d, msg:%s, len:%d\n", handle,
            str.c_str(), static_cast<int>(msg_len));
    return 0;
}

#define CHECK(e) \
    if ((e) != 0) {\
        printf("file:%s, line:%d, error:%s\n", __FILE__, __LINE__, lspf::net::Message::LastErrorMessage());\
        exit(1);\
    }

void thread_send(){

    char  msg1[100];

    for (int i=0; i<100000; i++){
        sprintf(msg1, "activemq message test[%d]", i);
        CHECK(lspf::net::Message::Send(0, msg1, strlen(msg1)));
        usleep(10);
    }
}

void Init()
{
    std::string brokerURI =
        "failover://(tcp://127.0.0.1:61616)";

    std::string destURI = "TEST.FOO";

    lspf::net::ATMQDriver driver(brokerURI, destURI);

    driver.Init(MessageDriver::MESSAGE_CLIENT);

    driver.SetOnMessage(on_message);

    lspf::net::Message::RegisterDriver(&driver);

    int handle = lspf::net::Message::NewHandle();

    CHECK(lspf::net::Message::Run(handle, &driver));

    printf("handle:%d\n", handle);

    boost::thread_group group;

    for(int num=0;num<2;num++)
		group.create_thread(boost::bind(&thread_send));
    group.join_all();
    // Wait to exit.
    std::cout << "Press 'q' to quit" << std::endl;
    while( std::cin.get() != 'q') {}
}

int main(int argc, char **argv) {
    //安装消息驱动
    lspf::net::ATMQDriver::Install();

    Init();

    //卸载驱动
    lspf::net::ATMQDriver::Uninstall();
    return 0;
}

