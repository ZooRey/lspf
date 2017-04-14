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

    //��鿨bin
    int CheckCardBin(const std::string &strCardNo);

    //����Ƿ��Զ������ѯ ֻ��ѯ�Զ���־
    int AutoSettMerch(const std::string &strMerchNo);

    int CheckAmount(const std::string &strMchntNo, const std::string &strCardNo, const int nAmount, const std::string &strWtype);

    //��ȡ���������޶���Ϣ
    int GetWhiteListLimit(const std::string &merch_no, const std::string &account_no, unsigned long &limit_amount);

    //��ȡ�̻����յȼ�
    int GetMerchRiskLevel(const std::string &merch_no, std::string &risk_level);

    //��ȡ���յȼ������޶�
    int GetRiskLevelLimit(const std::string &risk_level, unsigned long &limit_amount);

    //��ȡ�����콻���ܽ��
    int GetCardDayTotalAmout(const std::string &merch_no, const std::string &account_no, unsigned long &total_amount);
private:
    DBCommandPtr m_db_cmd;
};
#endif // __DAO_PAYMENT_H__
