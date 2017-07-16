#ifndef LSPF_NET_ACTIVEMQ_DRIVER_H
#define LSPF_NET_ACTIVEMQ_DRIVER_H

#include <string>
#include <map>
#include <queue>
#include <vector>
#include <boost/noncopyable.hpp>
#include "message_driver.h"

namespace lspf {
namespace net {

class ATMQHandler;

////////////////////////////////////////////////////////////////////////////////
class ATMQDriver : public boost::noncopyable, public MessageDriver
{
public:

    ATMQDriver( const std::string& brokerURI, const std::string& destURI,
                    bool needReply = true,bool useTopic = false, bool clientAck = false );

    virtual ~ATMQDriver();

    virtual int Init(DRIVER_TYPE type);

    virtual int Run(int handle, SendToFunction *send_to);

    /// @brief 启动服务
    virtual int Start();

    /// @brief 停止服务
    virtual int Stop();

    virtual int Close();

    virtual std::string Name() {
        return "activemq";
    }

    static void Install();

    static void Uninstall();

private:
    int SendTo(const char* msg, size_t msg_len, boost::shared_ptr<MessageReply> addr);

private:
	int m_handle;
	bool m_needReply;
    bool m_useTopic;
    bool m_clientAck;
    std::string m_brokerURI;
    std::string m_destURI;
	ATMQHandler *m_atmq_handler;
};


} /* namespace lspf */
} /* namespace net */

#endif
