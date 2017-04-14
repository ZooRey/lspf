#ifndef __BUSI_PAYMENT_H__
#define __BUSI_PAYMENT_H__
#include <string>
#include <vector>

#include "busi_handler.h"
#include "database/db_oracle.h"

class PayRecord;
class PropertyPayment;
class DAOPayment;

class PaymentHandler :public BusiHandler
{
public:
    PaymentHandler();
    ~PaymentHandler();

protected:
	virtual int process();
	virtual int parseJson(const std::string &strInput);
	virtual int makeJson(std::string &strOutput);
    //У��ǩ��
    virtual int verifySign();
private:
    //����Ԥ����
    void prepare();

    int checkExist();
    //��齻�׷��տ���
    void checkRiskCtrl();

    //��ȡ����·����Ϣ
    void getChlCode();

    //��ֽ��׽��
    void split();

    //���׼���
    void account();

    //���׻���
    void audit();

    //��������·��
    void route();

    void updateFailedResult();

private:
    bool routeHandle();

    void RouteToCitic();
private:
    int m_logId;

    //�������������־
    bool m_bInsertAcc;
    //�������������־
    bool m_bInsertDtl;

    int m_status;           //����״̬
    std::string m_ret_code; //����Ӧ����
    std::string m_ret_msg;

    DBCommandPtr m_db_connection;

    //�������Ϣ
    PropertyPayment *m_pProperty;
    //���ݱ����
    DAOPayment *m_pDAOPayment;

    PayRecord *m_pay_record;
};

#endif // __BUSI_PAYMENT_H__
