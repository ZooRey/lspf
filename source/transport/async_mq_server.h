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

    /// @brief �����߳�
    virtual void Run();

    /// @brief ��ʼ������
    /// @param brokerURI �����ַ
    /// @param destURI ��������
    /// @param need_reply �Ƿ���ҪӦ��
    /// @param use_topic �Ƿ�Ϊ����ģʽ
    /// @return true�ɹ� falseʧ��
    bool Init(std::string &brokerURI, std::string &destURI, bool need_reply=true, bool use_topic=false);
/*
    /// @brief ������Ϣ�����У�������ʹ�ã�
    /// @param send_message ��������
    /// @return true�ɹ� falseʧ��
    bool SendToQueue(const std::string &send_message);
*/
    /// @brief ����ʽ����Ϣ���н������ݣ�������ʹ�ã�
    /// @param recv_message ��������
    /// @return ��
    void RecvFromQueue(std::string &recv_message);

    /// @brief ����Ϣ���н������ݣ�������ʹ�ã�
    /// @param timeout ���ճ�ʱʱ��
    /// @param recv_message ��������
    /// @return true�ɹ� false��ʱ������
    bool TimedRecvFromQueue(const int timeout, std::string &recv_message);

    /// @brief ������Ϣ�����У��첽RPCӦ��ʹ�ã�
    /// @param session_id ��Ϣ�ỰID,����Ӧ����Ϣ
    /// @param send_message ��������
    /// @return true�ɹ� falseʧ��
    bool SendToQueue(const std::string &session_id, const std::string &send_message);

    /// @brief ����ʽ����Ϣ���н������ݣ��첽RPCӦ��ʹ�ã�
    /// @param session_id ��Ϣ�ػ�ID,����Ӧ����Ϣ
    /// @param recv_message ��������
    /// @return ��
    void RecvFromQueue(std::string &session_id, std::string &recv_message);

    /// @brief ����Ϣ���н������ݣ��첽RPCӦ��ʹ�ã�
    /// @param timeout ���ճ�ʱʱ��
    /// @param session_id ��Ϣ�ػ�ID,����Ӧ����Ϣ
    /// @param recv_message ��������
    /// @return true�ɹ� false��ʱ������
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

