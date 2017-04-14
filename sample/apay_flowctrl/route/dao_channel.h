#ifndef __DAO_CHANNEL_H__
#define __DAO_CHANNEL_H__

#include <vector>
#include <string>
#include "database/db_oracle.h"

struct __STR_CHANNEL_INFO
{
    std::string strChanellId;      //����ID
    std::string strWtVal;          //Ȩ��ֵ
};

class DAOChannel
{
public:
    DAOChannel(const int nId = 0);
    DAOChannel(DBCommandPtr db_cmd);
    ~DAOChannel();

    //��ȡ·�ɹ���ID
    bool GetRRuleId(std::vector<std::string> &vecRRuleId);

    //��ȡ����ID��Ӧ����󵥱�֧�����
    bool GetMaxSingleAmt(const std::string &strChannelId, std::string &strChannelName, unsigned long &nMaxSingleAmt);

    //��ȡ�Ƿ�Ϊ�����ձ�־
    bool GetWorkDayFlag(const std::string &pay_date, std::string &flag);

    bool GetChannelId(const std::string &strPayDt, const std::string &strWorkDayFlag, const int &iPayAmt, std::string &channel_id);

    bool GetChannelId(const int &amount, std::string &channel_id, std::string &channel_name);

private:
    std::vector<__STR_CHANNEL_INFO> m_vChlInfo;

    DBCommandPtr m_db_cmd;
};
#endif // __DAO_PAYMENT_H__
