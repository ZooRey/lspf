#ifndef LSPF_NET_TCP_DRIVER_h
#define LSPF_NET_TCP_DRIVER_h

#include <zmq.h>
#include <map>
#include <queue>
#include <vector>
#include "message_driver.h"

namespace lspf {
namespace net {

class TcpListenerThread;

class ZMQDriver : public MessageDriver {
public:
    ZMQDriver(const std::string &url);

    virtual ~ZMQDriver();

    virtual int Init(DRIVER_TYPE type);

    virtual int Run(int handle, SendToFunction *send_to);

    virtual int Close();

    virtual std::string Name() {
        return "zeromq";
    }

    int Poll(int max_event);

private:
    int Bind(int handle, SendToFunction *send_to);

    int Connect(int handle, SendToFunction *send_to);

    int BindPull();

    //×ª·¢
    int OnForward(zmq_msg_t &message);

    int OnServerMessage(zmq_msg_t &message);

private:
    int m_handle;

    bool m_isServer;

    std::string m_url;

    void *m_z_ctx;

    void *m_z_socket;

    void *m_pull_socket;

    TcpListenerThread *m_listen_thread;

    std::vector<zmq_pollitem_t> m_poll_items;

private:
    int tcpSendTo(void* z_socket, const char* msg,
            size_t msg_len, boost::shared_ptr<MessageReply> reply_addr);

    int tcpSend(void* z_socket, const char* msg, size_t msg_len);
};

} /* namespace lspf */
} /* namespace net */

#endif // LSPF_NET_TCP_DRIVER_h
