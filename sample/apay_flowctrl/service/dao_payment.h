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

    //��ȡ��Կ
    bool getRsaPubKey(const std::string strMchntNo, std::string &strPubKey, std::string &strMchntName);

    //��ȡ�������κ�
    bool getPayBatchNo(std::string &strPayBatchNo);

    //��ȡ������ˮID
    bool getPaySeqId(std::string &strPaySeqId);

    //���ô���ҵ�����ʹ���
    bool setTxnCode(PropertyPayment *pProperty);

    //����Ƿ��Զ������ѯ ֻ��ѯ�Զ���־
    int autoSettMerch(const std::string &strMerchNo);

    //�������������ˮ��ԭʼ������ˮ��
    bool insertPayACC(const PropertyPayment *pProperty);

    //�������������ˮ��
    bool insertPayDTL(const PropertyPayment *pProperty, const PayRecord *pRecord);

    //��ȡ�̻����
    bool getBalance(const std::string &strMerNo, std::string &strBalance);

    //��ȡ��bin��Ϣ
    bool getCardBin(const std::string &strCardNo, std::string &strBankCode, std::string &strBankName);

    bool getCardBin(const std::string &strCardNo, std::string &strBankCode, std::string &strBankName, std::string &strBankUnionNo);

    //��ȡ��������״̬
    //int getPaymentStatus(const std::string &strRequestId, std::string &strTime, int &nTotalAmt);
    int getPaymentStatus(const string &strRequestId, std::string &strTime, int &nTotalAmt,std::string &ret_msg);

    //��ȡԭ����������Ϣ������0�ɹ� -1����1��¼������
    int getPayACC(const std::string &strRequestId, std::string &strBatchNo, std::string &strAccountNo, std::string &strChannelId);

    //��ȡ��������ID���Ѳ�ֺ�ļ�¼��
    bool getPayDTL(const std::string &strRequestId, std::vector<PayRecord *> &vecPayAccId);

    //��ȡ������ˮ����״̬
    int getPayDtlStatus(const std::string &strPayAccId);

    //���������������ˮ��ԭʼ������ˮ��
    bool insertPayACCRST(const std::string &strRequestId);

    //���´�����������ˮ�������루ԭʼ������ˮ��
    bool updatePayACC(const std::string &strRequestId,  const int status, const std::string &ret_code, const std::string &ret_msg);

    //���������������ˮ��ԭʼ������ˮ��
    bool insertPayDTLRST(const std::string &strPayAccId);

    //���´�����������ˮ�������루��ֺ���ˮ��
    bool updatePayDTL(const std::string &strPayAccId, const int status, const std::string &ret_code, const std::string & ret_msg);

    //�������д�����������ˮ�������루��ֺ���ˮ��
    bool updateAllPayDTLRST(const std::string &strRequestId,  const int status, const std::string &ret_code);

    //��ȡT+1��������
    bool getPayDate(std::string &strPayDate, const std::string &strWithdrawDate, const std::string &strWType="1");

    //������Ϣ���
    int InsertIntoCitic(const std::string &pay_id, const PropertyPayment *property);
private:
	DBCommandPtr m_db_cmd;
};
#endif // __DAO_PAYMENT_H__
