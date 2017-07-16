#ifndef LSPF_NET_MESSAGEDRIVER_h
#define LSPF_NET_MESSAGEDRIVER_h

#include <stdint.h>
#include <string.h>
#include <string>
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include <boost/shared_ptr.hpp>

namespace lspf {
namespace net {

class MessageReply{
public:
    MessageReply(){}
    virtual~MessageReply(){}
};

typedef boost::function<int (const char* msg, size_t msg_len)> SendFunction;

typedef boost::function<int (const char* msg, size_t msg_len, boost::shared_ptr<MessageReply>)> SendToFunction;

typedef boost::function<int (int, const char*, size_t, boost::shared_ptr<MessageReply>)> OnMessage;


/// @brief 网络驱动统一接口
class MessageDriver {
public:

    enum DRIVER_TYPE{
        MESSAGE_CLIENT,
        MESSAGE_SERVER
    };
    virtual ~MessageDriver() {}

    virtual int Init(DRIVER_TYPE type) = 0;

    virtual int Run(int handle, SendToFunction *send_to) = 0;

    /// @brief 启动服务
    virtual int Start(){ return 0;}

    /// @brief 停止服务
    virtual int Stop(){ return 0;}

    /// @brief 关闭网络驱动
    virtual int Close() = 0;

    /// @brief 获取网络驱动类型
    /// @return 返回网络驱动类型名称
    virtual std::string Name() = 0;

    void SetOnMessage(const OnMessage &on_message){
        m_on_message = on_message;
    }
protected:
    OnMessage m_on_message;
};

} /* namespace lspf */
} /* namespace net */

#endif // PEBBLE_NET_MESSAGEDRIVER_h
