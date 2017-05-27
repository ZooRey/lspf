#ifndef __RPC_CLIENT_MQ_H__
#define __RPC_CLIENT_MQ_H__

#include "common/thread.h"
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

    virtual ~AsyncMQClient(){}

    /// @brief �����߳�
    virtual void Run();

    /// @brief ��ʼ������
    /// @param brokerURI �����ַ
    /// @param destURI ��������
    /// @param need_reply �Ƿ���ҪӦ��
    /// @param use_topic �Ƿ�Ϊ����ģʽ
    /// @return true�ɹ� falseʧ��
    bool Init(std::string &brokerURI, std::string &destURI, bool need_reply=true, bool use_topic=false);

    /// @brief ������Ϣ������
    /// @param send_message ��������
    /// @return true�ɹ� falseʧ��
    bool SendToQueue(const std::string &send_message);

    /// @brief ����ʽ����Ϣ���н������ݣ��첽RPCʹ�ã�
    /// @param recv_message ��������
    /// @return ��
    void RecvFromQueue(std::string &recv_message);

    /// @brief ����Ϣ���н������ݣ��첽RPCʹ�ã�
    /// @param timeout ���ճ�ʱʱ��
    /// @param recv_message ��������
    /// @return true�ɹ� false��ʱ������
    bool TimedRecvFromQueue(const int timeout, std::string &recv_message);

private:
    AsyncMQClient(){}

    int m_client_handle;

    static AsyncMQClient *m_async_mq_client;
};

#endif

