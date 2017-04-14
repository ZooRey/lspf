#include "risk_control.h"
#include <memory>
#include <iostream>
#include <string>
#include "log/log.h"
#include "database/db_oracle.h"
#include "dao_riskctrl.h"


using namespace std;


RiskControl::RiskControl(const int amount, const std::string &merch_no, const std::string &account_no, const std::string &txn_type)
    :m_amount(amount), m_merch_no(merch_no), m_account_no(account_no), m_txn_type(txn_type)
{
    m_dao_riskctrl =  new DAORiskCtrl(0);
}
RiskControl::RiskControl(const int amount, const std::string &merch_no, const std::string &account_no, const std::string &txn_type, DBCommandPtr db_cmd)
    :m_amount(amount), m_merch_no(merch_no), m_account_no(account_no), m_txn_type(txn_type), m_dao_riskctrl(new DAORiskCtrl(db_cmd))
{

}


RiskControl::~RiskControl()
{
    if (m_dao_riskctrl != NULL)
    {
        delete m_dao_riskctrl;
    }
}

//业务处理
int RiskControl::CheckRisk()
{   int rc = 0;
    PLOG_DEBUG( "RiskControl m_account_no = [%s]", m_account_no.c_str());
    PLOG_DEBUG( "RiskControl m_amount = [%d]", m_amount);

    rc = CheckCardDayLimit();

    PLOG_DEBUG( "RiskControl CheckRisk = [%d]", rc);
    return rc;
}

//检查卡单日提现限制
int RiskControl::CheckCardDayLimit()
{
    int rc = 0;
    unsigned long card_total_amount = 0;
    unsigned long max_card_day_amount = 0;

    //获取卡白名单限额信息
    rc = m_dao_riskctrl->GetWhiteListLimit(m_merch_no, m_account_no, max_card_day_amount);
    if (rc < 0){
        return -1;
    }else if(rc > 0){
        std::string risk_level;
        //获取商户风险等级
        rc = m_dao_riskctrl->GetMerchRiskLevel(m_merch_no, risk_level);
        if (rc < 0){
            return -1;
        }
        //获取风险等级提现限额
        rc = m_dao_riskctrl->GetRiskLevelLimit(risk_level, max_card_day_amount);
        if (rc < 0){
            return -1;
        }
    }

    rc = m_dao_riskctrl->GetCardDayTotalAmout(m_merch_no, m_account_no, card_total_amount);
    if (rc < 0){
        return -1;
    }

    if (card_total_amount >= max_card_day_amount){
        return 1;
    }

    PLOG_DEBUG( "RiskControl CheckRisk = [%d]", rc);
    return rc;
}

