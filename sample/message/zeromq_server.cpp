#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <boost/thread/thread.hpp>
#include "common/thread_pool.h"
#include "message/message.h"
#include "message/zmq_driver.h"

using namespace std;
using namespace lspf::net;

#define CHECK(e) \
    if ((e) != 0) {\
        printf("file:%s, line:%d, error:%s\n", __FILE__, __LINE__, Message::LastErrorMessage());\
        exit(1);\
    }

ThreadPool ThreadPool_;

void thread_send(int handle, const char *msg, size_t msg_len, boost::shared_ptr<MessageReply> addr){
    string str(msg, msg_len);
    printf("server handle:%d, msg:%s, len:%d\n", handle,
            str.c_str(), static_cast<int>(msg_len));

    char ret_msg[512];
    int ret_n = snprintf(ret_msg, sizeof(ret_msg), "your msg=%s", str.c_str());
    CHECK(Message::SendTo(handle, ret_msg, ret_n, addr));
}

int on_message(int handle, const char *msg, size_t msg_len, boost::shared_ptr<MessageReply> addr) {

    int64_t i = 100;
    boost::function<void()> fun =boost::bind(&thread_send, handle, msg, msg_len, addr);
    ThreadPool_.AddTask(fun, i);

    return 0;
}


int main(int argc, char **argv) {

    ZMQDriver driver("tcp://0.0.0.0:8260");

    ThreadPool_.Init(4);

    driver.Init(MessageDriver::MESSAGE_SERVER);

    Message::RegisterDriver(&driver);

    int handle = Message::NewHandle();

    printf("handle:%d\n", handle);

    driver.SetOnMessage(on_message);

    Message::Run(handle, &driver);

    // Wait to exit.
    std::cout << "Press 'q' to quit" << std::endl;
    while( std::cin.get() != 'q') {}

    return 0;
}

