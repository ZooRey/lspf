#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "message/message.h"
#include "message/atmq_driver.h"

using namespace std;
using namespace lspf::net;


#define CHECK(e) \
    if ((e) != 0) {\
        printf("file:%s, line:%d, error:%s\n", __FILE__, __LINE__, lspf::net::Message::LastErrorMessage());\
        exit(1);\
    }
int on_message(int handle, const char *msg, size_t msg_len, boost::shared_ptr<lspf::net::MessageReply> addr) {
    string str(msg, msg_len);
    printf("server handle:%d, msg:%s, len:%d\n", handle,
            str.c_str(), static_cast<int>(msg_len));

    char ret_msg[512];
    int ret_n = snprintf(ret_msg, sizeof(ret_msg), "your msg=%s", str.c_str());
    CHECK(lspf::net::Message::SendTo(handle, ret_msg, ret_n, addr));
    return 0;
}

void Init()
{
    std::string brokerURI = "failover://(tcp://172.20.11.100:61616)";

    std::string destURI = "FINANCE.ACC";

    lspf::net::ATMQDriver driver(brokerURI, destURI);

    driver.Init(MessageDriver::MESSAGE_SERVER);

    lspf::net::Message::RegisterDriver(&driver);

    driver.SetOnMessage(on_message);

    int handle = lspf::net::Message::NewHandle();

    CHECK(lspf::net::Message::Run(handle, &driver));

    printf("handle:%d\n", handle);

    // Wait to exit.
    std::cout << "Press 'q' to quit" << std::endl;
    while( std::cin.get() != 'q') {}
}

int main(int argc, char **argv) {

    lspf::net::ATMQDriver::Install();

    Init();

    lspf::net::ATMQDriver::Uninstall();

    return 0;
}

