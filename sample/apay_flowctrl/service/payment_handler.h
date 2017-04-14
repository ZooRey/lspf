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
    //校验签名
    virtual int verifySign();
private:
    //数据预处理
    void prepare();

    int checkExist();
    //检查交易风险控制
    void checkRiskCtrl();

    //获取渠道路由信息
    void getChlCode();

    //拆分交易金额
    void split();

    //交易记账
    void account();

    //交易稽核
    void audit();

    //交易渠道路由
    void route();

    void updateFailedResult();

private:
    bool routeHandle();

    void RouteToCitic();
private:
    int m_logId;

    //插入代付请求表标志
    bool m_bInsertAcc;
    //插入代付受理表标志
    bool m_bInsertDtl;

    int m_status;           //交易状态
    std::string m_ret_code; //交易应答码
    std::string m_ret_msg;

    DBCommandPtr m_db_connection;

    //付款交易信息
    PropertyPayment *m_pProperty;
    //数据表访问
    DAOPayment *m_pDAOPayment;

    PayRecord *m_pay_record;
};

#endif // __BUSI_PAYMENT_H__
