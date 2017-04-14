#ifndef ACCPAY_APP_EXCEPTION_H__
#define ACCPAY_APP_EXCEPTION_H__

#include "mexception.h"

//应用异常错误类
class AppException : public MException {
public:
    /**
    * 应用自定义错误码.
    */
    enum AppExceptionType {
        UNKNOWN = 0,      //未知
        INVALID_SIGN = 1, //签名校验错误
        LACK_BALANCE = 2,     //机构余额不足
        RECORD_EXISTED = 3, //记录已存在
        PAYMENT_FAILED = 4, //付款失败
        BANK_PROCESSING = 5,//银行正在处理
        RECORD_NOTFOUND = 6,     //原记录不存在
        RISKCTRL_FAILED = 7,     //风控阻止交易
        INTERNAL_ERROR=8 //内部错误
    };

    AppException() : MException(), type_(UNKNOWN) {}

    AppException(AppExceptionType type) : MException(), type_(type) {}

    AppException(const std::string& message) : MException(message), type_(UNKNOWN) {}

    AppException(AppExceptionType type, const std::string& message)
        : MException(message), type_(type) {}

    virtual ~AppException() throw() {}

    /**
    * @return 错误码
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
    * 错误码
    */
    AppExceptionType type_;
}; //ConnectionException

#endif
