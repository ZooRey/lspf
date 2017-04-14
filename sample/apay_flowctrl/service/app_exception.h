#ifndef ACCPAY_APP_EXCEPTION_H__
#define ACCPAY_APP_EXCEPTION_H__

#include "mexception.h"

//Ӧ���쳣������
class AppException : public MException {
public:
    /**
    * Ӧ���Զ��������.
    */
    enum AppExceptionType {
        UNKNOWN = 0,      //δ֪
        INVALID_SIGN = 1, //ǩ��У�����
        LACK_BALANCE = 2,     //��������
        RECORD_EXISTED = 3, //��¼�Ѵ���
        PAYMENT_FAILED = 4, //����ʧ��
        BANK_PROCESSING = 5,//�������ڴ���
        RECORD_NOTFOUND = 6,     //ԭ��¼������
        RISKCTRL_FAILED = 7,     //�����ֹ����
        INTERNAL_ERROR=8 //�ڲ�����
    };

    AppException() : MException(), type_(UNKNOWN) {}

    AppException(AppExceptionType type) : MException(), type_(type) {}

    AppException(const std::string& message) : MException(message), type_(UNKNOWN) {}

    AppException(AppExceptionType type, const std::string& message)
        : MException(message), type_(type) {}

    virtual ~AppException() throw() {}

    /**
    * @return ������
    */
    AppExceptionType getType() const { return type_; }

    virtual const char* what() const throw() {
        if (message_.empty()) {
            switch (type_) {
            case UNKNOWN:
                return "AppException: Unkown";
            case INVALID_SIGN:
                return "AppException: Invalid sign";
            case LACK_BALANCE:
                return "AppException: Lack balance ";
            case RECORD_EXISTED:
                return "AppException: The pay record is existed";
            case PAYMENT_FAILED:
                return "AppException: Payment failed";
            case BANK_PROCESSING:
                return "AppException: Bank processing";
            case RECORD_NOTFOUND:
                return "AppException: The pay record is not found";
            case INTERNAL_ERROR:
                return "AppException: Internal error";
            case RISKCTRL_FAILED:
                return "AppException: Risk control";
            default:
                return "AppException: (Invalid exception type)";
            };
        } else {
            return message_.c_str();
        }
    }

protected:
    /**
    * ������
    */
    AppExceptionType type_;
}; //ConnectionException

#endif
