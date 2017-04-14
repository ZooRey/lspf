#ifndef LSPF_RPC_CONNECTION_EXCEPTION_H__
#define LSPF_RPC_CONNECTION_EXCEPTION_H__

#include "mexception.h"

namespace lspf{
namespace rpc{

//RPC连接池操作异常类
class ConnectionException : public MException {
public:
  /**
   * 数据库操作自定义错误码.
   */
  enum ConnectionExceptionType {
    UNKNOWN = 0,
    UNKNOWN_CONNECTION_TYPE = 1, //无法识别的连接类型
    INVALID_PARAM = 2,     //非法参数,无效的IP和端口
    INVALID_POOL_NAME = 3, //无效连接池名称
    BAD_CONNECTION_POOL = 4, //出错的连接池
    INVALID_CONNECTION = 5,//无效连接
    CONNECT_FAILED = 6,     //连接失败
    INTERNAL_ERROR = 7,     //内部错误
	NON_ENOUGH_CONNECTION=8 //连接数已满，无法获取连接
  };

  ConnectionException() : MException(), type_(UNKNOWN) {}

  ConnectionException(ConnectionExceptionType type) : MException(), type_(type) {}

  ConnectionException(const std::string& message) : MException(message), type_(UNKNOWN) {}

  ConnectionException(ConnectionExceptionType type, const std::string& message)
    : MException(message), type_(type) {}

  virtual ~ConnectionException() throw() {}

  /**
   * @return 错误码
   */
  ConnectionExceptionType getType() { return type_; }

  virtual const char* what() const throw() {
    if (message_.empty()) {
      switch (type_) {
      case UNKNOWN:
        return "ConnectionException: Unknown exception";
      case UNKNOWN_CONNECTION_TYPE:
        return "ConnectionException: Unknown Connection Type";
      case INVALID_PARAM:
        return "ConnectionException: Invalid Param ip or port ";
      case BAD_CONNECTION_POOL:
        return "ConnectionException: Bad Connection pool";
      case INVALID_POOL_NAME:
        return "ConnectionException: Internal pool name";
      case INVALID_CONNECTION:
        return "ConnectionException: Invalid connection";
      case CONNECT_FAILED:
        return "ConnectionException: Connection failed";
      case INTERNAL_ERROR:
        return "ConnectionException: Internal error";
      case NON_ENOUGH_CONNECTION:
        return "ConnectionException: Non-enough connection";
      default:
        return "ConnectionException: (Invalid exception type)";
      };
    } else {
      return message_.c_str();
    }
  }


protected:
  /**
   * 错误码
   */
  ConnectionExceptionType type_;
}; //ConnectionException


}//namespace rpc
}//namespace lspf

#endif
