#ifndef ROUTE_CALCULATOR_H__
#define ROUTE_CALCULATOR_H__

#include <string>

#include "database/db_oracle.h"

class DAOChannel;

class RouteCalculator
{
public:
    RouteCalculator(int amount, const std::string &txn_code, const std::string &pay_date);
    RouteCalculator(int amount, const std::string &txn_code, const std::string &pay_date, DBCommandPtr db_cmd);
    ~RouteCalculator();

public:
    //获取渠道ID对应的相关信息
    int GetChannelInfo(const std::string &account_type, const std::string &txn_type, std::string &channel_id, std::string &channel_name);

    static void SetChannelBal(const std::string &channel_id, unsigned long balance);

    static unsigned long &GetChannelBal(const std::string &channel_id);
private:
    int m_amount;
    std::string m_txn_code;
    std::string m_pay_date;
    std::string m_merch_no;
    std::string m_account_no;
    std::string m_bank_code;
    std::string m_channel_id;
    std::string m_channel_name;

    DAOChannel *m_pDAOChannel;

    //暂时记录及判断民生渠道余额
    static unsigned long channel_bal;
};

#endif // __PAY_MENT_H__
