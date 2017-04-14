#ifndef __QUERY_HANDLER_H__
#define __QUERY_HANDLER_H__

class DAOPayment;
class PayRecord;

#include <string>
#include <vector>
#include "busi_handler.h"

class QueryHandler :public BusiHandler
{
public:
    QueryHandler();
    ~QueryHandler();

protected:
	virtual int process();
	virtual int parseJson(const std::string &strInput);
	virtual int makeJson(std::string &strOutput);
    //У��ǩ��
    virtual int verifySign();
private:

    //�������ݱ��ѯ���
    int localQuery();

    //ͨ��������ѯ���
    int channelQuery();

private:
    int channelQueryHandle(const std::string &strPayId, const std::string &strBatchNo,
                           const std::string &strAccountNo, const std::string &strChannelId);

private:
    std::string m_strVersion;
    std::string m_strRequestId;
    std::string m_strPay_chl_idd;
    std::string m_strOrganizeCode;
    std::string m_strOrgRequestId;
    std::string m_strQueryTime;
    std::string m_strMemo;
    std::string m_strSign;
private:
    //ԭʼ�����ܽ��
    int m_nTotalAmt;
    //��ֽ��׼�¼
    std::vector<PayRecord *> m_vecPayRecord;

    //���ݱ����
    DAOPayment *m_pDAOPayment;
private:
    int m_nRetCode;
    std::string m_strRetMsg;
};

#endif // __BUSI_PAYMENT_H__
