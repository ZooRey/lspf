#ifndef LSPF_NET_MESSAGE_EXCEPTION_H__
#define LSPF_NET_MESSAGE_EXCEPTION_H__

#include "mexception.h"

namespace lspf{
namespace net{

//��Ϣ�����쳣��
class MessageException : public MException {
public:
  /**
   * ��Ϣ�����Զ��������.
   */
  enum MessageExceptionType {
    UNKNOWN = 0,
    UNKNOWN_DRIVER = 1, //�޷�ʶ�����������
    INVALID_PARAM = 2,     //�Ƿ�����,��Ч��IP�Ͷ˿�
    INVALID_POOL_NAME = 3, //��Ч���ӳ�����
    BAD_CONNECTION_POOL = 4, //��������ӳ�
    INVALID_CONNECTION = 5,//��Ч����
    CONNECT_FAILED = 6,     //����ʧ��
    INTERNAL_ERROR = 7,     //�ڲ�����
	NON_ENOUGH_CONNECTION=8 //�������������޷���ȡ����
  };

  MessageException() : MException(), type_(UNKNOWN) {}

  MessageException(MessageExceptionType type) : MException(), type_(type) {}

  MessageException(const std::string& message) : MException(message), type_(UNKNOWN) {}

  MessageException(MessageExceptionType type, const std::string& message)
    : MException(message), type_(type) {}

  virtual ~MessageException() throw() {}

  /**
   * @return ������
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
   * ������
   */
  MessageExceptionType type_;
}; //MessageException


}//namespace rpc
}//namespace lspf

#endif
