#ifndef __DAO_PAYMENT_H__
#define __DAO_PAYMENT_H__

#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include "database/db_oracle.h"

class PayRecord;
class PropertyPayment;

class DAOPayment
{
public:
    DAOPayment(const int nId = 0);
    DAOPayment(DBCommandPtr db_cmd);
    ~DAOPayment();

    //获取公钥
    bool getRsaPubKey(const std::string strMchntNo, std::string &strPubKey, std::string &strMchntName);

    //获取代付批次号
    bool getPayBatchNo(std::string &strPayBatchNo);

    //获取代付流水ID
    bool getPaySeqId(std::string &strPaySeqId);

    //设置代付业务类型代码
    bool setTxnCode(PropertyPayment *pProperty);

    //检查是否自动结算查询 只查询自动标志
    int autoSettMerch(const std::string &strMerchNo);

    //插入代付请求流水表（原始请求流水）
    bool insertPayACC(const PropertyPayment *pProperty);

    //插入代付受理流水表
    bool insertPayDTL(const PropertyPayment *pProperty, const PayRecord *pRecord);

    //获取商户余额
    bool getBalance(const std::string &strMerNo, std::string &strBalance);

    //获取卡bin信息
    bool getCardBin(const std::string &strCardNo, std::string &strBankCode, std::string &strBankName);

    bool getCardBin(const std::string &strCardNo, std::string &strBankCode, std::string &strBankName, std::string &strBankUnionNo);

    //获取代付请求状态
    //int getPaymentStatus(const std::string &strRequestId, std::string &strTime, int &nTotalAmt);
    int getPaymentStatus(const string &strRequestId, std::string &strTime, int &nTotalAmt,std::string &ret_msg);

    //获取原代付请求信息，返回0成功 -1错误，1记录不存在
    int getPayACC(const std::string &strRequestId, std::string &strBatchNo, std::string &strAccountNo, std::string &strChannelId);

    //获取代付受理ID（已拆分后的记录）
    bool getPayDTL(const std::string &strRequestId, std::vector<PayRecord *> &vecPayAccId);

    //获取代付流水交易状态
    int getPayDtlStatus(const std::string &strPayAccId);

    //插入代付请求结果流水表（原始请求流水）
    bool insertPayACCRST(const std::string &strRequestId);

    //更新代付请求结果流水表结果代码（原始请求流水）
    bool updatePayACC(const std::string &strRequestId,  const int status, const std::string &ret_code, const std::string &ret_msg);

    //插入代付请求结果流水表（原始请求流水）
    bool insertPayDTLRST(const std::string &strPayAccId);

    //更新代付受理结果流水表结果代码（拆分后流水）
    bool updatePayDTL(const std::string &strPayAccId, const int status, const std::string &ret_code, const std::string & ret_msg);

    //更新所有代付受理结果流水表结果代码（拆分后流水）
    bool updateAllPayDTLRST(const std::string &strRequestId,  const int status, const std::string &ret_code);

    //获取T+1付款日期
    bool getPayDate(std::string &strPayDate, const std::string &strWithdrawDate, const std::string &strWType="1");

    //代付信息入库
    int InsertIntoCitic(const std::string &pay_id, const PropertyPayment *property);
private:
	DBCommandPtr m_db_cmd;
};
#endif // __DAO_PAYMENT_H__
