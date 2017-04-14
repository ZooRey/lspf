#include "zmq_driver.h"
#include <string.h>
#include "message.h"
#include <stdlib.h>
#include <limits>
#include <assert.h>
#include <iostream>
#include "common/mutex.h"
#include "common/thread.h"

using namespace std;
namespace lspf { namespace net{

typedef union {
    struct {
        char peer_id[6];
        uint16_t peer_id_size;
    } address;
    uint64_t u64;
} ZMQAddressHelper;


class ZMQReply : public MessageReply{
public:
    ZMQReply(){};
    virtual ~ZMQReply(){
        std::cout << "~ZMQReply" <<std::endl;
    }

    ZMQAddressHelper addr;
};

class TcpListenerThread : public Thread{
public:
    TcpListenerThread(ZMQDriver *driver)
        : m_driver(driver){
    }
    ~TcpListenerThread(){
        //std::cout << "~TcpListenerThread():" << std::endl;
    }
    virtual void Run(){
        while(true){
            m_driver->Poll(100);
            usleep(100);
        }
    }

private:
    ZMQDriver *m_driver;
};

ZMQDriver::ZMQDriver(const std::string &url) :
    m_isServer(false),
    m_url(url.substr(6)),
    m_z_ctx(NULL),
    m_z_socket(NULL),
    m_pull_socket(NULL),
    m_listen_thread(NULL) {
}

ZMQDriver::~ZMQDriver(){
    if (m_listen_thread != NULL){
        delete m_listen_thread;
    }
}

int ZMQDriver::Init(DRIVER_TYPE type) {

    switch (type){
    case MESSAGE_CLIENT:
        m_isServer = false;
        break;
    case MESSAGE_SERVER:
        m_isServer = true;
        break;
    default:
        m_isServer = false;
        break;
    }

    m_z_ctx = zmq_init(1);
    return 0;
}

int ZMQDriver::tcpSendTo(void* z_socket, const char* msg,
        size_t msg_len, boost::shared_ptr<MessageReply> reply_addr) {

    if (!m_isServer)
    {
        return tcpSend(z_socket, msg, msg_len);
    }

    boost::shared_ptr<ZMQReply> reply = boost::dynamic_pointer_cast<ZMQReply>(reply_addr);

    ZMQAddressHelper addr_helper ;
    addr_helper.u64 = reply->addr.u64;

    if (addr_helper.address.peer_id_size > 6) {
        NET_LIB_ERR_RETURN("Address Invalid");
    }

    int rc;

    void *push = zmq_socket(m_z_ctx, ZMQ_PUSH);

    rc = zmq_connect(push, "inproc://a");

    if (rc != 0) {
        zmq_close(push);
        ERRNO_RETURN();
    }

    rc = zmq_send(push, addr_helper.address.peer_id,
            addr_helper.address.peer_id_size, ZMQ_SNDMORE | ZMQ_DONTWAIT);
    if (rc < 0) {
        zmq_close(push);
        ERRNO_RETURN();
    }

    rc = zmq_send(push, msg, msg_len, ZMQ_DONTWAIT);
    if (rc < 0) {
        zmq_close(push);
        ERRNO_RETURN();
    }

    zmq_close(push);

    return 0;
}

int ZMQDriver::tcpSend(void* z_socket, const char* msg, size_t msg_len) {

    int rc;

    std::cout << "push" << std::endl;
    void *push = zmq_socket(m_z_ctx, ZMQ_PUSH);

    rc = zmq_connect(push, "inproc://a");
    if (rc != 0) {
        std::cout << "zmq_connect failed" << std::endl;
        zmq_close(push);
        ERRNO_RETURN();

    }
    std::cout << "zmq_connect success" << std::endl;

    zmq_msg_t empty;
    rc = zmq_msg_init(&empty);
    if (0 != rc) {
        zmq_close(push);
        ERRNO_RETURN();
    }

    rc = zmq_sendmsg(push, &empty, ZMQ_SNDMORE | ZMQ_DONTWAIT);
    if (rc < 0) {
        zmq_close(push);
        ERRNO_RETURN();
    }

    rc = zmq_send(push, msg, msg_len, ZMQ_DONTWAIT);
    if (rc < 0) {
        zmq_close(push);
        ERRNO_RETURN();
    }

    zmq_close(push);

    return 0;
}

int ZMQDriver::Bind(int handle, SendToFunction *send_to) {
    int spos = m_url.find(":");
    if (spos <= 0) {
        NET_LIB_ERR_RETURN("URL Invalid");
    }

    int iport = atoi(m_url.substr(spos + 1).c_str());
    if (iport < 0 || iport > std::numeric_limits<uint16_t>::max()) {
        NET_LIB_ERR_RETURN("Port Invalid");
    }
    int rc;

    m_handle = handle;
    m_z_socket = zmq_socket(m_z_ctx, ZMQ_ROUTER);

    int high_water = 10000;
    zmq_setsockopt(m_z_socket, ZMQ_SNDHWM, &high_water, sizeof(high_water));
    zmq_setsockopt(m_z_socket, ZMQ_RCVHWM, &high_water, sizeof(high_water));

    rc = zmq_bind(m_z_socket, ("tcp://" + m_url).c_str());

    if (rc != 0) {
        zmq_close(m_z_socket);
        ERRNO_RETURN();
    }

    *send_to = boost::bind(&ZMQDriver::tcpSendTo, this, m_z_socket, _1, _2, _3);

    zmq_pollitem_t poll_item;
    poll_item.socket = m_z_socket;
    poll_item.events = ZMQ_POLLIN;
    poll_item.fd = 0;
    m_poll_items.push_back(poll_item);

    return 0;
}


int ZMQDriver::Connect(int handle, SendToFunction *send_to) {
    int spos = m_url.find(":");
    if (spos <= 0) {
        NET_LIB_ERR_RETURN("URL Invalid");
    }

    int iport = atoi(m_url.substr(spos + 1).c_str());
    if (iport <= 0 || iport > std::numeric_limits<uint16_t>::max()) {
        NET_LIB_ERR_RETURN("Port Invalid");
    }

    int rc;

    m_handle = handle;
    m_z_socket = zmq_socket(m_z_ctx, ZMQ_DEALER);

    int high_water = 10000;
    zmq_setsockopt(m_z_socket, ZMQ_SNDHWM, &high_water, sizeof(high_water));
    zmq_setsockopt(m_z_socket, ZMQ_RCVHWM, &high_water, sizeof(high_water));

    rc = zmq_connect(m_z_socket, ("tcp://" + m_url).c_str());

    if (rc != 0) {
        zmq_close(m_z_socket);
        ERRNO_RETURN();
    }


    *send_to = boost::bind(&ZMQDriver::tcpSendTo, this, m_z_socket, _1, _2, _3);

    zmq_pollitem_t poll_item;
    poll_item.socket = m_z_socket;
    poll_item.events = ZMQ_POLLIN;
    poll_item.fd = 0;
    m_poll_items.push_back(poll_item);

    return 0;
}


int ZMQDriver::Close() {

    if (m_z_socket == NULL) {
        return -1;
    } else {
        typedef std::vector<zmq_pollitem_t>::iterator Iterator;
        for (Iterator iter = m_poll_items.begin();
                iter != m_poll_items.end(); iter++) {
            if ((*iter).socket == m_z_socket) {
                m_poll_items.erase(iter);
                break;
            }
        }

        int rc = zmq_close(m_z_socket);
        if (rc != 0) {
            ERRNO_RETURN();
        } else {
            return 0;
        }
    }
}

int ZMQDriver::BindPull() {

    int rc;

    m_pull_socket = zmq_socket(m_z_ctx, ZMQ_PULL);

    int high_water = 10000;
    zmq_setsockopt(m_pull_socket, ZMQ_SNDHWM, &high_water, sizeof(high_water));
    zmq_setsockopt(m_pull_socket, ZMQ_RCVHWM, &high_water, sizeof(high_water));

    rc = zmq_bind(m_pull_socket, "inproc://a");

    if (rc != 0) {
        zmq_close(m_pull_socket);
        return rc;
    }

    zmq_pollitem_t poll_item;
    poll_item.socket = m_pull_socket;
    poll_item.events = ZMQ_POLLIN;
    poll_item.fd = 0;
    m_poll_items.push_back(poll_item);

    return 0;
}

//×ª·¢
int ZMQDriver::OnForward(zmq_msg_t &message){

    int n = 0;
    int rc;

    void *source_socket = m_pull_socket;
    void *dest_socket = m_z_socket;

    while (true){
        rc = zmq_msg_recv(&message, source_socket, ZMQ_DONTWAIT);
        if (rc < 0) {
            ERRNO_RETURN();
        }

        if (zmq_msg_more(&message)) {

            rc = zmq_sendmsg(dest_socket, &message, ZMQ_SNDMORE | ZMQ_DONTWAIT);
            if (rc < 0) {
                ERRNO_RETURN();
            }

            while (true) { // discard message
                rc = zmq_msg_recv(&message, source_socket, ZMQ_DONTWAIT);
                if (rc < 0) {
                    ERRNO_RETURN();
                }else if (!zmq_msg_more(&message)){
                    break;
                }else{
                    rc = zmq_sendmsg(dest_socket, &message, ZMQ_SNDMORE | ZMQ_DONTWAIT);
                    if (rc < 0) {
                        ERRNO_RETURN();
                    }
                }
            }
        }

        rc = zmq_sendmsg(dest_socket, &message, ZMQ_DONTWAIT);
        if (rc < 0) {
            ERRNO_RETURN();
        }

        if (++n >= 100) {
            break;
        }
    }

    return 0;
}

int ZMQDriver::OnServerMessage(zmq_msg_t &message)
{
    int rc;
    ZMQAddressHelper addr_helper;

    // no our message
    if (zmq_msg_size(&message) > sizeof(addr_helper.address.peer_id)) {
        while (true) { // discard message
            rc = zmq_msg_recv(&message, m_z_socket, ZMQ_DONTWAIT);
            if (rc < 0 || !zmq_msg_more(&message)) {
                break;
            }
        }

        if (rc < 0) {
            return -1;
        } else {
            return 0;
        }
    }

    addr_helper.address.peer_id_size = zmq_msg_size(&message);
    memcpy(addr_helper.address.peer_id, zmq_msg_data(&message),
            addr_helper.address.peer_id_size);

    while (true) {
        rc = zmq_msg_recv(&message, m_z_socket, ZMQ_DONTWAIT);
        if (rc < 0) {
            break;
        }
        if (!zmq_msg_more(&message)) {
            break;
        }
    }

    if (rc >= 0) {
        if (m_on_message) {
            boost::shared_ptr<ZMQReply> reply(new ZMQReply());
            reply->addr.u64 = addr_helper.u64;

            m_on_message(m_handle,
                    reinterpret_cast<char*>(zmq_msg_data(&message)),
                    zmq_msg_size(&message), reply);
        }
    }

    return 0;
}

int ZMQDriver::Poll(int max_event) {

    int n = 0;
    int rc = 0;

    zmq_msg_t message;
    rc = zmq_msg_init(&message);
    if (rc != 0) {
        return 0;
    }

    while (true) {

        rc = zmq_poll(m_poll_items.data(), m_poll_items.size(), 0);

        if (rc <= 0) {
            usleep(100);
            continue;
        }

        for (uint32_t i = 0; i < m_poll_items.size(); i++) {

            if (m_poll_items[i].revents <= 0) {
                continue;
            }

            void *z_socket = m_poll_items[i].socket;

            if (m_pull_socket == z_socket){
                rc = OnForward(message);

                continue;
            }

            while (true) {
                rc = zmq_msg_recv(&message, z_socket, ZMQ_DONTWAIT);
                if (rc < 0) {
                    break;
                }

                if (zmq_msg_more(&message)) { // server side
                    rc = OnServerMessage(message);
                    if (rc < 0 ){
                        break;
                    }
                } else { // client side
                    if (m_on_message) {
                        m_on_message(m_handle,
                                reinterpret_cast<char*>(zmq_msg_data(&message)),
                                zmq_msg_size(&message), boost::shared_ptr<ZMQReply>());
                    }
                }

                if (++n >= max_event) {
                    break;
                }
            }

        }
    }

    zmq_msg_close(&message);
    return n;
}

int ZMQDriver::Run(int handle, SendToFunction *send_to){
    int rc = 0;
    if (m_isServer)
        rc = Bind(handle, send_to);
    else
        rc = Connect(handle, send_to);

    if(rc < 0){
        return rc;
    }

    rc = BindPull();
    if(rc < 0){
        return rc;
    }

    if (m_listen_thread == NULL){
        m_listen_thread = new TcpListenerThread(this);
        m_listen_thread->Start();
    }

    return 0;
}


}
}
