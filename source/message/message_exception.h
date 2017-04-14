#ifndef LSPF_NET_MESSAGE_EXCEPTION_H__
#define LSPF_NET_MESSAGE_EXCEPTION_H__

#include "mexception.h"

namespace lspf{
namespace net{

//消息操作异常类
class MessageException : public MException {
public:
  /**
   * 消息操作自定义错误码.
   */
  enum MessageExceptionType {
    UNKNOWN = 0,
    UNKNOWN_DRIVER = 1, //无法识别的驱动类型
    INVALID_PARAM = 2,     //非法参数,无效的IP和端口
    INVALID_POOL_NAME = 3, //无效连接池名称
    BAD_CONNECTION_POOL = 4, //出错的连接池
    INVALID_CONNECTION = 5,//无效连接
    CONNECT_FAILED = 6,     //连接失败
    INTERNAL_ERROR = 7,     //内部错误
	NON_ENOUGH_CONNECTION=8 //连接数已满，无法获取连接
  };

  MessageException() : MException(), type_(UNKNOWN) {}

  MessageException(MessageExceptionType type) : MException(), type_(type) {}

  MessageException(const std::string& message) : MException(message), type_(UNKNOWN) {}

  MessageException(MessageExceptionType type, const std::string& message)
    : MException(message), type_(type) {}

  virtual ~MessageException() throw() {}

  /**
   * @return 错误码
   */
  MessageExceptionType getType() { return type_; }

  virtual const char* what() const throw() {
    if (message_.empty()) {
      switch (type_) {
      case UNKNOWN:
        return "MessageException: Unknown exception";
      case UNKNOWN_CONNECTION_TYPE:
        return "MessageException: Unknown Connection Type";
      case INVALID_PARAM:
        return "MessageException: Invalid Param ip or port ";
      case BAD_CONNECTION_POOL:
        return "MessageException: Bad Connection pool";
      case INVALID_POOL_NAME:
        return "MessageException: Internal pool name";
      case INVALID_CONNECTION:
        return "MessageException: Invalid connection";
      case CONNECT_FAILED:
        return "MessageException: Connection failed";
      case INTERNAL_ERROR:
        return "MessageException: Internal error";
      case NON_ENOUGH_CONNECTION:
        return "MessageException: Non-enough connection";
      default:
        return "MessageException: (Invalid exception type)";
      };
    } else {
      return message_.c_str();
    }
  }


protected:
  /**
   * 错误码
   */
  MessageExceptionType type_;
}; //MessageException


}//namespace rpc
}//namespace lspf

#endif
