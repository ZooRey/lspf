#ifndef __DAO_RISKCTRL_H__
#define __DAO_RISKCTRL_H__

#include <string>
#include "database/db_oracle.h"

class DAORiskCtrl
{
public:
    DAORiskCtrl(const int nId = 0);
    DAORiskCtrl(DBCommandPtr db_cmd);
    ~DAORiskCtrl();

    //检查卡bin
    int CheckCardBin(const std::string &strCardNo);

    //检查是否自动结算查询 只查询自动标志
    int AutoSettMerch(const std::string &strMerchNo);

    int CheckAmount(const std::string &strMchntNo, const std::string &strCardNo, const int nAmount, const std::string &strWtype);

    //获取卡白名单限额信息
    int GetWhiteListLimit(const std::string &merch_no, const std::string &account_no, unsigned long &limit_amount);

    //获取商户风险等级
    int GetMerchRiskLevel(const std::string &merch_no, std::string &risk_level);

    //获取风险等级提现限额
    int GetRiskLevelLimit(const std::string &risk_level, unsigned long &limit_amount);

    //获取卡当天交易总金额
    int GetCardDayTotalAmout(const std::string &merch_no, const std::string &account_no, unsigned long &total_amount);
private:
    DBCommandPtr m_db_cmd;
};
#endif // __DAO_PAYMENT_H__
