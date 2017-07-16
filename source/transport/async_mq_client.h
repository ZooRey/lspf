#ifndef ASYNC_MQ_CLIENT
#define ASYNC_MQ_CLIENT

#include "common/thread.h"

#include "common/condition_variable.h"
#include "common/blocking_queue.h"
#include "message/message.h"
#include "message/atmq_driver.h"

#include <string>
class AsyncMQClient : public Thread
{
public:

    static AsyncMQClient *Instance(){
        if (m_async_mq_client == NULL){
            m_async_mq_client = new AsyncMQClient;
        }
        return m_async_mq_client;
    }

    AsyncMQClient();

    AsyncMQClient(BlockingQueue<std::string> *recv_queue);

    virtual ~AsyncMQClient(){}

    /// @brief 启动线程
    virtual void Run();

    /// @brief 初始化队列
    /// @param brokerURI 服务地址
    /// @param destURI 队列名称
    /// @param need_reply 是否需要应答
    /// @param use_topic 是否为主题模式
    /// @return true成功 false失败
    bool Init(std::string &brokerURI, std::string &destURI, bool need_reply=true, bool use_topic=false);

    /// @brief 发送消息到队列
    /// @param send_message 发送数据
    /// @return true成功 false失败
    bool SendToQueue(const std::string &send_message);

    /// @brief 阻塞式从消息队列接收数据（异步RPC使用）
    /// @param recv_message 接收数据
    /// @return 无
    void RecvFromQueue(std::string &recv_message);

    /// @brief 从消息队列接收数据（异步RPC使用）
    /// @param timeout 接收超时时间
    /// @param recv_message 接收数据
    /// @return true成功 false超时无数据
    bool TimedRecvFromQueue(const int timeout, std::string &recv_message);

    int OnAsyncMessage(int handle, const char *message, size_t message_len, boost::shared_ptr<lspf::net::MessageReply> reply);
private:
    int m_client_handle;
    BlockingQueue<std::string> *m_send_queue;
    BlockingQueue<std::string> *m_recv_queue;

    lspf::net::ATMQDriver *m_atmqdriver;

    static AsyncMQClient *m_async_mq_client;
};

#endif

