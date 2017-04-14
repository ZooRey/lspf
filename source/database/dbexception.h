#ifndef __DB_EXCEPTION_H__
#define __DB_EXCEPTION_H__

#include "mexception.h"

//���ݿ����ӳز����쳣�࣬��OCCI���SQLException�޹�
class DBException : public MException {
public:
  /**
   * ���ݿ�����Զ��������.
   */
  enum DBExceptionType {
    UNKNOWN = 0,
    ENVIRONMENT_ERROR = 1, //��������
    INVALID_PARAM = 2,     //�Ƿ�����,��Ч���û��������롢������
    INVALID_CONNECTION_POOL = 3,
    INVALID_CONNECTION = 4,
    INVALID_STATEMENT = 5,
    INTERNAL_ERROR = 6,
    PROTOCOL_ERROR = 7,
    INVALID_TRANSFORM = 8,
    INVALID_PROTOCOL = 9,
    UNSUPPORTED_CLIENT_TYPE = 10
  };

  DBException() : MException(), type_(UNKNOWN) {}

  DBException(DBExceptionType type) : MException(), type_(type) {}

  DBException(const std::string& message) : MException(message), type_(UNKNOWN) {}

  DBException(DBExceptionType type, const std::string& message)
    : MException(message), type_(type) {}

  virtual ~DBException() throw() {}

  /**
   * @return ������
   */
  DBExceptionType getType() { return type_; }

  virtual const char* what() const throw() {
    if (message_.empty()) {
      switch (type_) {
      case UNKNOWN:
        return "DBException: Unknown database exception";
      case ENVIRONMENT_ERROR:
        return "DBException: Environment error";
      case INVALID_CONNECTION_POOL:
        return "MDBException: Invalid connection pool";
      case INVALID_CONNECTION:
        return "MDBException: Invalid connection";
      case INVALID_STATEMENT:
        return "MDBException: Invalid Statement";
      case INTERNAL_ERROR:
        return "MDBException: Internal error";
      case PROTOCOL_ERROR:
        return "MDBException: Protocol error";
      case INVALID_TRANSFORM:
        return "MDBException: Invalid transform";
      case INVALID_PROTOCOL:
        return "MDBException: Invalid protocol";
      case UNSUPPORTED_CLIENT_TYPE:
        return "MDBException: Unsupported client type";
      default:
        return "MDBException: (Invalid exception type)";
      };
    } else {
      return message_.c_str();
    }
  }


protected:
  /**
   * ������
   */
  DBExceptionType type_;
};

#endif
