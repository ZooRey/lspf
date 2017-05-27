#include "async_mq_server.h"
#include "session_cache.h"
#include "log/log.h"
#include "common/string_utility.h"
#include "common/time_utility.h"
#include "message/message.h"
#include "message/atmq_driver.h"
using namespace lspf::net;
#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>

struct TransMessage{
    std::string msg_id;
    std::string msg_buffer;
};

//static boost::atomic_int server_seq_no(0);

static BlockingQueue<TransMessage> server_send_queue;
static BlockingQueue<TransMessage> server_recv_queue;

typedef boost::shared_ptr<MessageReply> MessageReplyPtr;
static SessionCache<MessageReplyPtr> server_session_cache;

AsyncMQServer *AsyncMQServer::m_async_mq_server = NULL;

static int OnAsyncRpcMessage(int handle, const char *message, size_t message_len, boost::shared_ptr<MessageReply> reply)
{
    PLOG_INFO("Get Message form MQ, length=%d, content=%s ",message_len, message);

    TransMessage trans_message;
    trans_message.msg_id = AsyncMQServer::Instance()->GenSessionId();
    trans_message.msg_buffer = std::string(message, message_len);

    AsyncMQServer::Instance()->SaveSession(trans_message.msg_id, reply);

    server_recv_queue.PushBack(trans_message);

    return 0;
}

bool AsyncMQServer::Init(std::string &brokerURI, std::string &destURI, bool need_reply, bool use_topic)
{
    static ATMQDriver *m_atmqdriver;
    bool clientAck = true;

    m_need_reply = need_reply;

    m_server_handle = lspf::net::Message::NewHandle();

    m_atmqdriver = new ATMQDriver(brokerURI, destURI, need_reply, use_topic, clientAck);

    m_atmqdriver->Init(MessageDriver::MESSAGE_CLIENT);

    m_atmqdriver->SetOnMessage(OnAsyncRpcMessage);

    lspf::net::Message::RegisterDriver(m_atmqdriver);
    lspf::net::Message::Run(m_server_handle, m_atmqdriver);


    return true;
}

void AsyncMQServer::Run()
{
    if (!m_need_reply){
        return;
    }

    TransMessage trans_message;
    boost::shared_ptr<MessageReply> reply;
	while(1){
        server_send_queue.PopFront(&trans_message);
        if (m_need_reply){
            GetSession(trans_message.msg_id, reply);
        }else{
            reply.reset();
        }
         ///发送数据到消息队列
        lspf::net::Message::SendTo(m_server_handle, trans_message.msg_buffer.c_str(), trans_message.msg_buffer.size(), reply);
    }
}

std::string AsyncMQServer::GenSessionId()
{
    if(!m_need_reply){
        return "";
    }

    static int server_seq_no = 0;
    std::string session_id;

    server_seq_no = ++server_seq_no > 99999999 ? 1 : server_seq_no;
    session_id = TimeUtility::GetStringTime() + StringUtility::IntToString(++server_seq_no);

    return session_id;
}

bool AsyncMQServer::SaveSession(const std::string &session_id, boost::shared_ptr<MessageReply> reply)
{
    if(!m_need_reply){
        return true;
    }

    return server_session_cache.PushSession(session_id, reply);
}

bool AsyncMQServer::GetSession(const std::string &session_id, boost::shared_ptr<MessageReply> reply)
{
    if(!m_need_reply){
        return true;
    }

    return server_session_cache.PopSession(session_id, &reply);
}
/*
bool AsyncMQServer::SendToQueue(const std::string &send_message)
{
    bool res;

    TransMessage trans_message;
    trans_message.msg_id = "";
    trans_message.msg_buffer = send_message;

    server_send_queue.TryPushBack(trans_message);
}
*/
void AsyncMQServer::RecvFromQueue(std::string &recv_message)
{
    TransMessage trans_message;
    server_recv_queue.PopFront(&trans_message);
    recv_message = trans_message.msg_buffer;
}

bool AsyncMQServer::TimedRecvFromQueue(const int timeout, std::string &recv_message)
{
    bool res = false;
    TransMessage trans_message;

    if (timeout > 0){
        res = server_recv_queue.TimedPopFront(&trans_message, timeout);
    }

    if (res){
        recv_message = trans_message.msg_buffer;
    }

    return res;
}


bool AsyncMQServer::SendToQueue(const std::string &msg_id, const std::string &send_message)
{
    TransMessage trans_message;
    trans_message.msg_id = msg_id;
    trans_message.msg_buffer = send_message;

    return server_send_queue.TryPushBack(trans_message);
}

void AsyncMQServer::RecvFromQueue(std::string &msg_id, std::string &recv_message)
{

    TransMessage trans_message;

    server_recv_queue.PopFront(&trans_message);

    recv_message = trans_message.msg_buffer;
}

bool AsyncMQServer::TimedRecvFromQueue(const int timeout, std::string &msg_id, std::string &recv_message)
{
    bool res = false;
    TransMessage trans_message;

    if (timeout > 0){
        res = server_recv_queue.TimedPopFront(&trans_message, timeout);
    }

    if (res){
        msg_id = trans_message.msg_id;
        recv_message = trans_message.msg_buffer;
    }

    return res;
}
