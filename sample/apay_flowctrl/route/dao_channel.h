#ifndef __DAO_CHANNEL_H__
#define __DAO_CHANNEL_H__

#include <vector>
#include <string>
#include "database/db_oracle.h"

struct __STR_CHANNEL_INFO
{
    std::string strChanellId;      //渠道ID
    std::string strWtVal;          //权重值
};

class DAOChannel
{
public:
    DAOChannel(const int nId = 0);
    DAOChannel(DBCommandPtr db_cmd);
    ~DAOChannel();

    //获取路由规则ID
    bool GetRRuleId(std::vector<std::string> &vecRRuleId);

    //获取渠道ID对应的最大单笔支付金额
    bool GetMaxSingleAmt(const std::string &strChannelId, std::string &strChannelName, unsigned long &nMaxSingleAmt);

    //获取是否为工作日标志
    bool GetWorkDayFlag(const std::string &pay_date, std::string &flag);

    bool GetChannelId(const std::string &strPayDt, const std::string &strWorkDayFlag, const int &iPayAmt, std::string &channel_id);

    bool GetChannelId(const int &amount, std::string &channel_id, std::string &channel_name);

private:
    std::vector<__STR_CHANNEL_INFO> m_vChlInfo;

    DBCommandPtr m_db_cmd;
};
#endif // __DAO_PAYMENT_H__
