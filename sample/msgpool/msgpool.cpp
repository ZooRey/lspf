#include "common/message_pool.h"
#include "common/string_utility.h"
#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <cstdio>

using namespace std;

struct Message
{
    std::string id;
    std::string context;
};


static MessagePool<Message*> msg_pool;

void ThreadFun(int id)
{
    Message *message = new Message;

    message->id = StringUtility::IntToString(id);
    message->context = "context is :" + message->id;

    msg_pool.SetMessage(message);

    sleep(10);

    Message *message_(NULL);

    msg_pool.GetMessage(&message_);

    std::cout << "ThreadFun id : " << id << ";  context: " << message_->context << std::endl;
}

void Test()
{
    sleep(3);

    for (size_t i=0; i<msg_pool.Size(); i++){
        Message *message;

        msg_pool.GetMessage(i, &message);
        std::cout << "SetMessage id : " << message->id << ";  context: " << message->context << std::endl;

        message->context += ", id=" + message->id;

        msg_pool.SetMessage(message);
    }

}

int main(int argc, char* argv[])
{
    boost::thread_group group;

    for(int num=1;num<100;num++)
        group.create_thread(boost::bind(&ThreadFun, num));

    group.create_thread(boost::bind(&Test));
    group.join_all();

	return 0;
}
