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
static BlockingQueue<TransMessage> server_recv_queue(5);

typedef boost::shared_ptr<MessageReply> MessageReplyPtr;
static SessionCache<MessageReplyPtr, std::string> mq_server_session_cache;

AsyncMQServer *AsyncMQServer::m_async_mq_server = NULL;

int AsyncMQServer::OnAsyncMessage(int handle, const char *message, size_t message_len, boost::shared_ptr<MessageReply> reply)
{
    static int64_t recvcout = 0;
    PLOG_DEBUG("Get Message from MQ, length=%d, content=%s, count=%d",message_len, message, ++recvcout);

    TransMessage trans_message;
    trans_message.msg_id = GenSessionId();
    trans_message.msg_buffer = std::string(message, message_len);

    if (m_need_reply){
/*
        while (mq_server_session_cache.Size() >= m_max_message_size){
            usleep(100);
        }
*/
        SaveSession(trans_message.msg_id, reply);
    }

    server_recv_queue.PushBack(trans_message);

    if(m_waitFun){
        while(!m_waitFun()){
            usleep(100);
        }
    }
/*
    while(!m_driver_staus){
        sleep(1);
    }
*/
    return 0;
}

bool AsyncMQServer::Init(std::string &brokerURI, std::string &destURI, bool need_reply, bool use_topic)
{
    if (m_driver_staus){
        return false;
    }

    bool clientAck = true;

    m_need_reply = need_reply;

    m_atmqdriver = new ATMQDriver(brokerURI, destURI, need_reply, use_topic, clientAck);

    m_server_handle = lspf::net::Message::NewHandle();

    m_atmqdriver->Init(MessageDriver::MESSAGE_SERVER);

    m_atmqdriver->SetOnMessage(boost::bind(&AsyncMQServer::OnAsyncMessage, this, _1, _2, _3, _4));

    lspf::net::Message::RegisterDriver(m_atmqdriver);
    lspf::net::Message::Run(m_server_handle, m_atmqdriver);

    m_driver_staus = true;

    return true;
}

void AsyncMQServer::StartService()
{
    if (m_atmqdriver != NULL){
        m_atmqdriver->Start();
        m_driver_staus = true;
    }
}

void AsyncMQServer::StopService()
{
    if (m_atmqdriver != NULL){
        m_driver_staus = false;
        sleep(5);  //休眠60秒
        m_atmqdriver->Stop();
    }
}


void AsyncMQServer::SetTimeout(const int timeout)
{
    m_session_timeout = timeout;
}


void AsyncMQServer::SetMaxMessageSize(const int max_size)
{
    m_max_message_size = max_size;
}

void AsyncMQServer::Run()
{
    if (!m_need_reply){
        return;
    }

    TransMessage trans_message;
    boost::shared_ptr<MessageReply> reply;

    int64_t last_time = TimeUtility::GetCurremtMs();

	while(1){

        if (server_send_queue.TimedPopFront(&trans_message, m_session_timeout * 1000)){
            reply.reset();
            GetSession(trans_message.msg_id, &reply);

            if (!m_driver_staus || reply.get() == NULL){
                PLOG_ERROR("msg_id=%s ",trans_message.msg_id.c_str());
                continue;
            }
             ///发送数据到消息队列
            lspf::net::Message::SendTo(m_server_handle, trans_message.msg_buffer.c_str(), trans_message.msg_buffer.size(), reply);
        }

        if (TimeUtility::GetCurremtMs() > last_time + m_session_timeout * 1000){
            ///60秒清除一次超时
            mq_server_session_cache.ClearTimeout(m_session_timeout);
            last_time = TimeUtility::GetCurremtMs();
        }
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

    return mq_server_session_cache.PushSession(session_id, reply);
}

bool AsyncMQServer::GetSession(const std::string &session_id, boost::shared_ptr<MessageReply> *reply)
{
    if(!m_need_reply){
        return true;
    }

    return mq_server_session_cache.PopSession(session_id, reply);
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

    msg_id = trans_message.msg_id;
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

void AsyncMQServer::SetWaitFunction(WaitFunction func)
{
    m_waitFun = func;
}
