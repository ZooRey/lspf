#ifndef __RPOPERTY_PAYMENT_H__
#define __RPOPERTY_PAYMENT_H__

#include <string>

class PayRecord
{
public:
    PayRecord(){};
    ~PayRecord(){};

    void setPayAmt(const int nPayAmt)
    {
        m_nPayAmt = nPayAmt;
    }

    int getPayAmt()const
    {
        return m_nPayAmt;
    }

    void setPayFee(const int nPayFee)
    {
        m_nPayFee = nPayFee;
    }

    int getPayFee()const
    {
        return m_nPayFee;
    }

    void setErrCode(const std::string &strErrCode)
    {
        m_strErrCode = strErrCode;
    }

    std::string getErrCode()const
    {
        return m_strErrCode;
    }

    void setPayId(const std::string &strPayId)
    {
        m_strPayId = strPayId;
    }

    std::string getPayId()const
    {
        return m_strPayId;
    }

    void setAuditStatus(const std::string &strAuditStatus)
    {
        m_strAuditStatus = strAuditStatus;
    }

    std::string getAuditStatus()const
    {
        return m_strAuditStatus;
    }

private:
    //拆分后交易金额
    int m_nPayAmt;
    //手续费
    int m_nPayFee;
     //应答码
    std::string m_strErrCode;
    //代付受理ID
    std::string m_strPayId;
    //稽核状态
    std::string m_strAuditStatus;
};


class PropertyPayment
{
public:
    PropertyPayment();
    ~PropertyPayment();

    void parseJson(const std::string &strInput);

    void makeJson(std::string &strOutput);

    void printRequset();
    void printResponse();
public:
    void setVersion(const std::string &strVersion);
    std::string getVersion()const;

    void setRequestId(const std::string &strRequestId);
    std::string getRequestId()const;

    void setOrganizeCode(const std::string &strOrganizeCode);
    std::string getOrganizeCode()const;

    void setStringAmount(const std::string &strAmount);
    std::string getStringAmount()const;

    void setAmount(const int nAmount);
    int getAmount()const;

    void setRecCardId(const std::string &strRecCardId);
    std::string getRecCardId()const;

    void setRecUserName(const std::string &strRecUserName);
    std::string getRecUserName()const;

    void setRecCardBank(const std::string &strRecCardBank);
    std::string getRecCardBank()const;

    void setRecCardSubBank(const std::string &strRecCardSubBank);
    std::string getRecCardSubBank()const;

    void setRecBankName(const std::string &strRecBankName);
    std::string getRecBankName()const;

    void setRequestTime(const std::string &strRequestTime);
    std::string getRequestTime()const;

    void setWType(const std::string &strWType);
    std::string getWType()const;

    void setWithdrawDate(const std::string &strWithdrawDate);
    std::string getWithdrawDate()const;

    void setAccountType(const std::string &account_type);
    std::string getAccountType()const;

    void setBankUnionNo(const std::string &bank_union_no);
    std::string getBankUnionNo()const;

    void setSign(const std::string &strSign);
	std::string getSign()const;

public:
    void setBatchNo(const std::string &strBatchNo);
    std::string getBatchNo()const;

    void setMchntNo(const std::string &strMchntNo);
	std::string getMchntNo()const;

    void setMchntName(const std::string &strMchntName);
	std::string getMchntName()const;

    void setRouteRuleId(const std::string &strRouteRuleId);
    std::string getRouteRuleId()const;

    void setChannelCode(const std::string &strChannelCode);
    std::string getChannelCode()const;

    //渠道名称
    void setChannelName(const std::string &strChannelName);
    std::string getChannelName()const;
    //单笔最大支付金额
    void setMaxSinglePayAmt(const int nMaxSinglePayAmt);
    int getMaxSinglePayAmt()const;

    //代付业务代码：D0，T1
    void setTxnCode(const std::string &strTxnCode);
    std::string getTxnCode()const;

    //代付方式：0单笔，1批量
    void setTxnType(const std::string &strTxnType);
    std::string getTxnType()const;

    //付款日期
    void setPayDate(const std::string &strPayDate);
    std::string getPayDate()const;

public:
    void setRetCode(const int nRetCode);
    int getRetCode() const;

    void setPayAccId(const std::string &strPayAccId);
    std::string getPayAccId() const;

    void setRetMsg(const std::string &strRetMsg);
    std::string getRetMsg() const;

private:
    int m_nAmount;
    std::string m_strVersion;
    std::string m_strRequestId;
    std::string m_strOrganizeCode;

    std::string m_strAmount;
    std::string m_strRecCardId;
    std::string m_strRecUserName;
    std::string m_strRecCardBank;
    std::string m_strRecCardSubBank;
    std::string m_strRecBankName;
    std::string m_strRequestTime;
    std::string m_strWType;
    std::string m_account_type;
    std::string m_bank_union_no;
    //提现申请日期
    std::string m_strWithdrawDate;
    //付款日期
    std::string m_strPayDate;
    std::string m_strMemo;
    std::string m_strSign;
private:
    std::string m_strBatchNo;
    //商户号
    std::string m_strMchntNo;
    //商户号
    std::string m_strMchntName;
    //路由规则ID
    std::string m_strRouteRuleId;
    //渠道代码
    std::string m_strChannelCode;
    //渠道名称
    std::string m_strChannelName;
    //单笔最大支付金额
    int m_nMaxSinglePayAmt;
    //代付业务代码：D0，T1
    std::string m_strTxnCode;
    //代付方式：0单笔，1批量
    std::string m_strTxnType;

private:
    int m_nRetCode;
    std::string m_strPayAccId;
    std::string m_strRetMsg;
};

//PropertyPaymentº¯Êý¶¨Òå

inline void PropertyPayment::setVersion(const std::string &strVersion)
{
	m_strVersion = strVersion;
}
inline std::string PropertyPayment::getVersion()const
{
	return m_strVersion;
}

inline void PropertyPayment::setRequestId(const std::string &strRequestId)
{
	m_strRequestId = strRequestId;
}

inline std::string PropertyPayment::getRequestId()const
{
	return m_strRequestId;
}

inline void PropertyPayment::setOrganizeCode(const std::string &strOrganizeCode)
{
	m_strOrganizeCode = strOrganizeCode;
}
inline std::string PropertyPayment::getOrganizeCode()const
{
	return m_strOrganizeCode;
}

inline void PropertyPayment::setStringAmount(const std::string &strAmount)
{
	m_strAmount = strAmount;
}
inline std::string PropertyPayment::getStringAmount()const
{
	return m_strAmount;
}

inline void PropertyPayment::setAmount(const int nAmount)
{
	m_nAmount = nAmount;
}
inline int PropertyPayment::getAmount()const
{
	return m_nAmount;
}

inline void PropertyPayment::setRecCardId(const std::string &strRecCardId)
{
	m_strRecCardId = strRecCardId;
}
inline std::string PropertyPayment::getRecCardId()const
{
	return m_strRecCardId;
}

inline void PropertyPayment::setRecUserName(const std::string &strRecUserName)
{
	m_strRecUserName = strRecUserName;
}
inline std::string PropertyPayment::getRecUserName()const
{
	return m_strRecUserName;
}

inline void PropertyPayment::setRecCardBank(const std::string &strRecCardBank)
{
	m_strRecCardBank = strRecCardBank;
}
inline std::string PropertyPayment::getRecCardBank()const
{
	return m_strRecCardBank;
}

inline void PropertyPayment::setRecCardSubBank(const std::string &strRecCardSubBank)
{
	m_strRecCardSubBank = strRecCardSubBank;
}
inline std::string PropertyPayment::getRecCardSubBank()const
{
	return m_strRecCardSubBank;
}

inline void PropertyPayment::setRecBankName(const std::string &strRecBankName)
{
	m_strRecBankName = strRecBankName;
}
inline std::string PropertyPayment::getRecBankName()const
{
	return m_strRecBankName;
}

inline void PropertyPayment::setRequestTime(const std::string &strRequestTime)
{
	m_strRequestTime = strRequestTime;
}
inline std::string PropertyPayment::getRequestTime()const
{
	return m_strRequestTime;
}

inline void PropertyPayment::setWType(const std::string &strWType)
{
	m_strWType = strWType;
}
inline std::string PropertyPayment::getWType()const
{
	return m_strWType;
}

inline void PropertyPayment::setWithdrawDate(const std::string &strWithdrawDate)
{
	m_strWithdrawDate = strWithdrawDate;
}
inline std::string PropertyPayment::getWithdrawDate()const
{
	return m_strWithdrawDate;
}

inline void PropertyPayment::setSign(const std::string &strSign)
{
	m_strSign = strSign;
}
inline std::string PropertyPayment::getSign()const
{
	return m_strSign;
}

inline void PropertyPayment::setBatchNo(const std::string &strBatchNo)
{
    m_strBatchNo = strBatchNo;
}

inline std::string PropertyPayment::getBatchNo()const
{
    return m_strBatchNo;
}

inline void PropertyPayment::setMchntNo(const std::string &strMchntNo)
{
	m_strMchntNo = strMchntNo;
}
inline std::string PropertyPayment::getMchntNo()const
{
	return m_strMchntNo;
}

inline void PropertyPayment::setMchntName(const std::string &strMchntName)
{
	m_strMchntName = strMchntName;
}
inline std::string PropertyPayment::getMchntName()const
{
	return m_strMchntName;
}

inline void PropertyPayment::setRouteRuleId(const std::string &strRouteRuleId)
{
	m_strRouteRuleId = strRouteRuleId;
}
inline std::string PropertyPayment::getRouteRuleId()const
{
	return m_strRouteRuleId;
}

inline void PropertyPayment::setChannelCode(const std::string &strChannelCode)
{
	m_strChannelCode = strChannelCode;
}
inline std::string PropertyPayment::getChannelCode()const
{
	return m_strChannelCode;
}

//渠道名称
inline void PropertyPayment::setChannelName(const std::string &strChannelName)
{
	m_strChannelName = strChannelName;
}
inline std::string PropertyPayment::getChannelName()const
{
	return m_strChannelName;
}

inline void PropertyPayment::setPayDate(const std::string &strPayDate)
{
	m_strPayDate = strPayDate;
}
inline std::string PropertyPayment::getPayDate()const
{
	return m_strPayDate;
}


//单笔最大支付金额
inline void PropertyPayment::setMaxSinglePayAmt(const int nMaxSinglePayAmt)
{
	m_nMaxSinglePayAmt = nMaxSinglePayAmt;
}
inline int PropertyPayment::getMaxSinglePayAmt()const
{
	return m_nMaxSinglePayAmt;
}

inline void PropertyPayment::setTxnCode(const std::string &strTxnCode)
{
	m_strTxnCode = strTxnCode;
}
inline std::string PropertyPayment::getTxnCode()const
{
	return m_strTxnCode;
}

//代付方式：0单笔，1批量
inline void PropertyPayment::setTxnType(const std::string &strTxnType)
{
	m_strTxnType = strTxnType;
}
inline std::string PropertyPayment::getTxnType()const
{
	return m_strTxnType;
}


inline void PropertyPayment::setRetCode(const int nRetCode)
{
    m_nRetCode = nRetCode;
}

inline int PropertyPayment::getRetCode() const
{
    return m_nRetCode;
}

inline void PropertyPayment::setRetMsg(const std::string &strRetMsg)
{
    m_strRetMsg = strRetMsg;
}

inline std::string PropertyPayment::getRetMsg() const
{
    return m_strRetMsg;
}

inline void PropertyPayment::setPayAccId(const std::string &strPayAccId)
{
    m_strPayAccId = strPayAccId;
}

inline std::string PropertyPayment::getPayAccId() const
{
    return m_strPayAccId;
}

inline void PropertyPayment::setAccountType(const std::string &account_type)
{
    m_account_type = account_type;
}
inline std::string PropertyPayment::getAccountType()const
{
    return m_account_type;
}

inline void PropertyPayment::setBankUnionNo(const std::string &bank_union_no)
{
    m_bank_union_no = bank_union_no;
}
inline std::string PropertyPayment::getBankUnionNo()const
{
    return m_bank_union_no;
}

#endif // __RPOPERTY_PAYMENT_H__
