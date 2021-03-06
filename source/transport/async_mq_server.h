#ifndef ASYNC_MQ_SERVER
#define ASYNC_MQ_SERVER

#include "common/thread.h"
#include "common/mutex.h"
#include "common/condition_variable.h"
#include "common/blocking_queue.h"
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include "message/message.h"
#include "message/atmq_driver.h"
using namespace lspf::net;

//从MQ中接收消息时等待的回调函数，如果返回为false将阻塞等待
typedef boost::function<bool ()> WaitFunction;

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

    void StartService();

    void StopService();

    /// @brief 设置通讯超时时间
    /// @param timeout 通讯超时时间（秒）
    /// @return 无
    void SetTimeout(const int timeout);

    /// @brief 设置最大处理消息数
    /// @param max_num 最大处理消息数
    /// @return 无
    void SetMaxMessageSize(const int max_size);
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

    /// @brief 设置从MQ中接收消息时等待的回调函数
    /// @param WaitFunction 回调函数
    /// @return 无
    void SetWaitFunction(WaitFunction func);

    int OnAsyncMessage(int handle, const char *message, size_t message_len, boost::shared_ptr<MessageReply> reply);
private:
    std::string GenSessionId();

    bool SaveSession(const std::string &session_id, boost::shared_ptr<MessageReply> reply);

    bool GetSession(const std::string &session_id, boost::shared_ptr<MessageReply> *reply);

private:
    AsyncMQServer() : m_server_handle(-1),
                      m_session_timeout(60),
                      m_max_message_size(100),
                      m_need_reply(false),
                      m_driver_staus(false), 
                      m_atmqdriver(NULL),
                      m_waitFun(NULL)
    {

    }
    int m_server_handle;
    int m_session_timeout;
    int m_max_message_size;

    bool m_need_reply;
    bool m_driver_staus;
    ATMQDriver *m_atmqdriver;
    WaitFunction m_waitFun;

    static AsyncMQServer *m_async_mq_server;
};

#endif

