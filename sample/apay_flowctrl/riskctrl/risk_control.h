#ifndef __RISK_CONTROL_H__
#define __RISK_CONTROL_H__

#include <string>
#include "database/db_oracle.h"

class DAORiskCtrl;

class RiskControl
{
public:
    RiskControl(const int amount, const std::string &merch_no, const std::string &account_no, const std::string &txn_type);
    RiskControl(const int amount, const std::string &merch_no, const std::string &account_no, const std::string &txn_type, DBCommandPtr db_cmd);
    ~RiskControl();

public:
    int CheckRisk();

private:
    //检查卡单日提现限制
    int CheckCardDayLimit();

private:
    int m_amount;              //交易金额
    std::string m_merch_no;       //商户号
    std::string m_account_no;     //收款账号
    std::string m_txn_type;         //交易类型(默认为0)

    DAORiskCtrl * m_dao_riskctrl;
};

#endif // __PAY_MENT_H__
