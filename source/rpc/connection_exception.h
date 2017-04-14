#ifndef LSPF_RPC_CONNECTION_EXCEPTION_H__
#define LSPF_RPC_CONNECTION_EXCEPTION_H__

#include "mexception.h"

namespace lspf{
namespace rpc{

//RPC���ӳز����쳣��
class ConnectionException : public MException {
public:
  /**
   * ���ݿ�����Զ��������.
   */
  enum ConnectionExceptionType {
    UNKNOWN = 0,
    UNKNOWN_CONNECTION_TYPE = 1, //�޷�ʶ�����������
    INVALID_PARAM = 2,     //�Ƿ�����,��Ч��IP�Ͷ˿�
    INVALID_POOL_NAME = 3, //��Ч���ӳ�����
    BAD_CONNECTION_POOL = 4, //��������ӳ�
    INVALID_CONNECTION = 5,//��Ч����
    CONNECT_FAILED = 6,     //����ʧ��
    INTERNAL_ERROR = 7,     //�ڲ�����
	NON_ENOUGH_CONNECTION=8 //�������������޷���ȡ����
  };

  ConnectionException() : MException(), type_(UNKNOWN) {}

  ConnectionException(ConnectionExceptionType type) : MException(), type_(type) {}

  ConnectionException(const std::string& message) : MException(message), type_(UNKNOWN) {}

  ConnectionException(ConnectionExceptionType type, const std::string& message)
    : MException(message), type_(type) {}

  virtual ~ConnectionException() throw() {}

  /**
   * @return ������
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
   * ������
   */
  ConnectionExceptionType type_;
}; //ConnectionException


}//namespace rpc
}//namespace lspf

#endif
