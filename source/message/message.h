#ifndef LSPF_NET_MESSAGE_h
#define LSPF_NET_MESSAGE_h

#include "message_driver.h"
#include <map>
#include <queue>
#include <set>
#include "common.h"

namespace lspf {
namespace net {
/// @brief 基于消息的通讯接口类
class Message {
public:
    // -------------------network api begin-------------------------
    /// @brief 创建一个名柄
    /// @param 无
    /// @return 成功：非负整数
    /// @return 失败：负数
    static int NewHandle();

    /// @brief 启动客户端模式（消费者）
    /// @param handle 由NewHandle返回的句柄
    /// @param driver 对MessageDriver接口实现的网络驱动
    /// @return 0 表示成功
    /// @return 非0 表示失败
    static int Run(int handle, MessageDriver * driver);

    /// @brief 服务端发送信息
    /// @param handle 由NewHandle返回的句柄
    /// @param msg 要发送的消息
    /// @param msg_len 消息的长度
    /// @param peer_addr 对端地址
    /// @return 0 表示成功
    /// @return 非0 表示失败
    static int SendTo(int handle, const char* msg,
            size_t msg_len, boost::shared_ptr<MessageReply> peer_addr);

    /// @brief 客户端发送消息
    /// @param handle 由NewHandle返回的句柄
    /// @param msg 要发送的消息
    /// @param msg_len 消息的长度
    /// @return 0 表示成功
    /// @return 非0 表示失败
    static int Send(int handle, const char* msg, size_t msg_len); // Client only


    /// @brief 启动句柄
    /// @param handle 由NewHandle返回
    /// @return 0 表示成功
    /// @return 非0 表示失败
    static int Start(int handle);

    /// @brief 启动句柄
    /// @param handle 由NewHandle返回
    /// @return 0 表示成功
    /// @return 非0 表示失败
    static int Stop(int handle);

    /// @brief 停止句柄
    /// @param handle 由NewHandle返回
    /// @return 0 表示成功
    /// @return 非0 表示失败
    static int Close(int handle);

    // error msg
    static char error_msg[256];

    static const char *p_error_msg;

    /// @brief 返回错误信息
    /// @param 无
    /// @return 返回错误描述
    static const char *LastErrorMessage();

    // -------------------network api end  -------------------------

public:
    /// @brief 注册通信协议驱动
    /// @param driver 对MessageDriver接口实现的网络驱动
    /// @return 0 表示成功
    /// @return -1 表示失败
    static int RegisterDriver(MessageDriver * driver);

    /// @brief 查询通信协议驱动
    /// @param handle 句柄
    /// @return NULL 句柄未使用
    /// @return 非空 该句柄绑定的驱动的指针
    static MessageDriver * QueryDriver(int handle);

private:
    static int CheckDriver(int handle, MessageDriver * driver);

    // functions
    static std::map<int, SendToFunction> send_to_functions;

    // handles
    static int32_t max_handle;

    static std::queue<int> free_handles;

    // drivers
    static std::map<std::string, MessageDriver*> prefix_to_driver;

    // handle to driver
    static std::map<int, MessageDriver*> handle_to_driver;

    // handles
    static std::set<MessageDriver*> all_driver;

    // handles
    static std::set<int> all_handles;

private:
    static int allocHandle();

    static void deallocHandle(int handle);

    static int sendToWrap(const SendFunction &send, const char* msg,
            size_t msg_len, boost::shared_ptr<MessageReply> peer_addr, int flag);
};

} /* namespace lspf */
} /* namespace net */

#endif // PEBBLE_NET_MESSAGE_h
