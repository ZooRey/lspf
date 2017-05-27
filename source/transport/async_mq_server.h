#ifndef __RPC_CLIENT_MQ_H__
#define __RPC_CLIENT_MQ_H__

#include "common/thread.h"
#include "common/mutex.h"
#include "common/condition_variable.h"
#include "common/blocking_queue.h"
#include <string>

#include <boost/shared_ptr.hpp>
#include "message/message.h"
#include "message/atmq_driver.h"
using namespace lspf::net;

class AsyncMQServer : public Thread
{
public:
    enum MQSERVER_TYPE{
        MQSERVER_CONSUMER = 0,
        MQSERVER_CONSUMER_RPC
    };

    static AsyncMQServer *Instance(){
        if (m_async_mq_server == NULL){
            m_async_mq_server = new AsyncMQServer;
        }
        return m_async_mq_server;
    }

    virtual ~AsyncMQServer(){}

    /// @brief 启动线程
    virtual void Run();

    /// @brief 初始化队列
    /// @param brokerURI 服务地址
    /// @param destURI 队列名称
    /// @param need_reply 是否需要应答
    /// @param use_topic 是否为主题模式
    /// @return true成功 false失败
    bool Init(std::string &brokerURI, std::string &destURI, bool need_reply=true, bool use_topic=false);
/*
    /// @brief 发送消息到队列（生产者使用）
    /// @param send_message 发送数据
    /// @return true成功 false失败
    bool SendToQueue(const std::string &send_message);
*/
    /// @brief 阻塞式从消息队列接收数据（消费者使用）
    /// @param recv_message 接收数据
    /// @return 无
    void RecvFromQueue(std::string &recv_message);

    /// @brief 从消息队列接收数据（消费者使用）
    /// @param timeout 接收超时时间
    /// @param recv_message 接收数据
    /// @return true成功 false超时无数据
    bool TimedRecvFromQueue(const int timeout, std::string &recv_message);

    /// @brief 发送消息到队列（异步RPC应答使用）
    /// @param session_id 消息会话ID,用于应答消息
    /// @param send_message 发送数据
    /// @return true成功 false失败
    bool SendToQueue(const std::string &session_id, const std::string &send_message);

    /// @brief 阻塞式从消息队列接收数据（异步RPC应答使用）
    /// @param session_id 消息回话ID,用于应答消息
    /// @param recv_message 接收数据
    /// @return 无
    void RecvFromQueue(std::string &session_id, std::string &recv_message);

    /// @brief 从消息队列接收数据（异步RPC应答使用）
    /// @param timeout 接收超时时间
    /// @param session_id 消息回话ID,用于应答消息
    /// @param recv_message 接收数据
    /// @return true成功 false超时无数据
    bool TimedRecvFromQueue(const int timeout, std::string &session_id, std::string &recv_message);

public:
    std::string GenSessionId();

    bool SaveSession(const std::string &session_id, boost::shared_ptr<MessageReply> reply);

    bool GetSession(const std::string &session_id, boost::shared_ptr<MessageReply> reply);

private:
    AsyncMQServer() : m_need_reply(true)
    {

    }

    bool m_need_reply;
    int m_server_handle;

    static AsyncMQServer *m_async_mq_server;
};

#endif

