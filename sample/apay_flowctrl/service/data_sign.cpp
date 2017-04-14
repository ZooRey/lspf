#include "data_sign.h"
#include "dao_payment.h"
#include "common/base64.h"
#include "log/log.h"
#include "../oldapi/toolkits4.h"
#include <iostream>

std::map<std::string, std::string> DataSign::rsa_key_list;

DataSign::DataSign(const int nMethod):m_nMethod(nMethod)
{

}
DataSign::~DataSign()
{

}

void DataSign::InitEncKey(const std::string &merch_no)
{
    if (merch_no.empty()){
        return;
    }

    std::string rsa_pub_key;
    std::string merch_name;

    DAOPayment dao_payment;
    //从数据库获取公钥
    if (!(dao_payment.getRsaPubKey(merch_no, rsa_pub_key, merch_name)))
    {
        PLOG_ERROR("error:getRsaPubKey failed");
        return;
    }

    rsa_key_list.insert(make_pair(merch_no, rsa_pub_key));
}

void DataSign::addItem(const std::string &strKey, const std::string &strValue)
{
    if (strValue.empty() &&
        (strKey == "wtype" || strKey == "memo"
         || strKey == "query_time" || strKey == "request_org_id"
         || strKey == "request_time" || strKey == "withdraw_date"))
    {
        return;
    }

    m_mapParam.insert(make_pair(strKey, strValue));
}

void DataSign::SetEncKey(const std::string &strEncKey)
{
    m_strEncKey = strEncKey;
}

bool DataSign::GetEncKey(const std::string &merch_no)
{
    if (rsa_key_list.find(merch_no) != rsa_key_list.end()){
        m_strEncKey = rsa_key_list[merch_no];
        return true;
    }

    return false;
}

bool DataSign::verify(const std::string &strBase64Sign)
{
    int iRet = 0;
    std::string data;
    std::string strSign;

    Base64::Decode(strBase64Sign, &strSign);

    data = getSignData();

    PLOG_DEBUG("data=[%s].", data.c_str());
    PLOG_DEBUG("rsa_key size=[%d].", m_strEncKey.size());

    iRet = RSA_PKVerify((unsigned char *)strSign.c_str(), strSign.size(),
                        (unsigned char *)data.c_str(), data.size(),
                        (unsigned char *)m_strEncKey.c_str(), m_strEncKey.size());
    if (iRet == 0)
    {
        return true;
    }
    else
    {
        return false;
    }

}

std::string DataSign::getSignData()
{
    std::string strData;
    // 将非空字段值，按照 key=value形式，用“，”链接起来
    std::map<std::string, std::string>::iterator it = m_mapParam.begin();
    for (; it != m_mapParam.end(); it++)
    {
        std::string key   = it->first;
        std::string value = it->second;

        if (!strData.empty())
        {
            strData += "&";
        }

        strData += key;
        strData += "=";
        strData += value;
    }

    return strData;
}

std::string DataSign::genSign()
{
    return "";
}

