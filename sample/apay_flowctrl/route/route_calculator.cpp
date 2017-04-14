#include "route_calculator.h"
#include <memory>
#include "common/app_conf.h"
#include "mexception.h"
#include "dao_channel.h"
#include "log/log.h"

unsigned long RouteCalculator::channel_bal = 99999999999;

RouteCalculator::RouteCalculator(int amount, const std::string &txn_code, const std::string &pay_date)
    :m_amount(amount), m_txn_code(txn_code), m_pay_date(pay_date)
{
    m_pDAOChannel = new DAOChannel(0);
}

RouteCalculator::RouteCalculator(int amount, const std::string &txn_code, const std::string &pay_date, DBCommandPtr db_cmd)
    :m_amount(amount), m_txn_code(txn_code), m_pay_date(pay_date), m_pDAOChannel(new DAOChannel(db_cmd))
{

}

RouteCalculator::~RouteCalculator()
{
    if (m_pDAOChannel != NULL)
    {
        delete m_pDAOChannel;
    }
}

void RouteCalculator::SetChannelBal(const std::string &channel_id, unsigned long balance)
{
    channel_bal = balance;
}

unsigned long &RouteCalculator::GetChannelBal(const std::string &channel_id)
{
    return channel_bal;
}

int RouteCalculator::GetChannelInfo(const std::string &account_type, const std::string &txn_type, std::string &channel_id, std::string &channel_name)
{
    std::string workday_flag;

    PLOG_DEBUG("GetChannelInfo pay_date:%s", m_pay_date.c_str());
    PLOG_DEBUG("GetChannelInfo txn_code:%s", m_txn_code.c_str());
    PLOG_DEBUG("GetChannelInfo amount:%d", m_amount);

    if (account_type == "1"){
        //对公账户路由选择
        channel_id = AppConf::Instance()->GetConfStr("Channel", "Channel_Company");
        if (!channel_id.empty()){
            return 0;
        }else{
            PLOG_ERROR("GetChannelId Company failed");
            return -1;
        }
    }

    if (m_txn_code == "1"){
        //T+1交易指定渠道
        channel_id = AppConf::Instance()->GetConfStr("Channel", "Channel_T1");
        if (!channel_id.empty()){
            return 0;
        }else{
            PLOG_ERROR("GetChannelId T+1 failed");
            return -1;
        }
    }
    if (!m_pDAOChannel->GetChannelId(m_amount, channel_id, channel_name)){
        PLOG_ERROR("GetChannelId failed");
        return -1;
    }

    PLOG_INFO("GetChannelInfo channel_id:%s, channel_name:%s", channel_id.c_str(), channel_name.c_str());

/*
    if (!m_pDAOChannel->GetWorkDayFlag(m_pay_date, workday_flag)){
        PLOG_ERROR("GetWorkDayFlag failed");
        return -1;
    }

    PLOG_DEBUG("GetWorkDayFlag:workday_flag=[%s]", workday_flag.c_str());

    if (!m_pDAOChannel->GetChannelId(m_pay_date, workday_flag, m_amount, channel_id)){
        PLOG_ERROR("GetChannelId failed");
        return -1;
    }

    PLOG_DEBUG("GetChannelId:channel_id=[%s]", channel_id.c_str());

    //获取渠道ID对应的最大单笔支付金额
    if (!m_pDAOChannel->GetMaxSingleAmt(channel_id, channel_name, max_amt)){
        PLOG_ERROR("GetMaxSingleAmt failed");
        return -1;
    }

    PLOG_DEBUG("GetWorkDayFlag:max_amt=[%d]", max_amt);

    PLOG_DEBUG("GetChannelInfo finish...");
*/
    return 0;
}
