#include "async_mq_client.h"
#include "common/mutex.h"
#include "common/condition_variable.h"
#include "common/blocking_queue.h"
#include "message/message.h"
#include "message/atmq_driver.h"
#include "log/log.h"
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
using namespace lspf::net;

AsyncMQClient *AsyncMQClient::m_async_mq_client = NULL;


AsyncMQClient::AsyncMQClient() : 
    m_send_queue(new BlockingQueue<std::string>), 
    m_recv_queue(new BlockingQueue<std::string>)
{

}

AsyncMQClient::AsyncMQClient(BlockingQueue<std::string> *recv_queue) : 
    m_send_queue(new BlockingQueue<std::string>), 
    m_recv_queue(recv_queue)
{
    
}

int AsyncMQClient::OnAsyncMessage(int handle, const char *message, size_t message_len, boost::shared_ptr<MessageReply> reply)
{
    PLOG_DEBUG("Get Message form MQ, length=%d, content=%s", message_len, message);
    m_recv_queue->PushBack(std::string(message, message_len));

    return 0;
}

bool AsyncMQClient::Init(std::string &brokerURI, std::string &destURI, bool need_reply, bool use_topic)
{
    bool clientAck = true;

    m_client_handle = lspf::net::Message::NewHandle();

    m_atmqdriver = new ATMQDriver(brokerURI, destURI, need_reply, use_topic, clientAck);

    m_atmqdriver->Init(MessageDriver::MESSAGE_CLIENT);

    if (need_reply){
        m_atmqdriver->SetOnMessage(boost::bind(&AsyncMQClient::OnAsyncMessage, this, _1, _2, _3, _4));
    }

    lspf::net::Message::RegisterDriver(m_atmqdriver);
    lspf::net::Message::Run(m_client_handle, m_atmqdriver);

    return true;
}

void AsyncMQClient::Run()
{
	std::string send_buff;
	while(1){
        m_send_queue->PopFront(&send_buff);

         ///发送数据到消息队列
        lspf::net::Message::Send(m_client_handle, send_buff.c_str(), send_buff.size());

        PLOG_DEBUG("Send to mq: length=%d, content=%s", send_buff.size(), send_buff.c_str());
    }
}

bool AsyncMQClient::SendToQueue(const std::string &send_message)
{
    return m_send_queue->TryPushBack(send_message);
}

void AsyncMQClient::RecvFromQueue(std::string &recv_message)
{
    m_recv_queue->PopFront(&recv_message);
}

bool AsyncMQClient::TimedRecvFromQueue(const int timeout, std::string &recv_message)
{
    return m_recv_queue->TimedPopFront(&recv_message, timeout);
}
