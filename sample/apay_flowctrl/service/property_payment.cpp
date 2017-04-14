#include "property_payment.h"

#include <iostream>
#include <stdlib.h>
#include <json/json.h>
#include "common/string_utility.h"

PropertyPayment::PropertyPayment() : m_nRetCode(0), m_strRetMsg("payment succeed...")
{
    //默认提现代码为D0
    m_strTxnCode.assign("D0");
}

PropertyPayment::~PropertyPayment()
{

}

void PropertyPayment::parseJson(const std::string &strInput)
{
    ///启用严格模式,不自动容错Json::Features::strictMode()
    ///不启用严格模式则需判断(value.type()==Json::objectValue)
    Json::Reader reader(Json::Features::strictMode());
    Json::Value value;

    if (reader.parse(strInput, value))
    {
        m_strVersion = value["version"].asString();
        m_strRequestId = value["request_id"].asString();
        m_strOrganizeCode = value["organize_no"].asString();
        m_strAmount = value["amount"].asString();
        m_strRecCardId = value["account_no"].asString();
        m_strRecUserName = value["account_name"].asString();
        m_account_type = value["account_type"].asString();
        m_strRecCardBank = value["bank_no"].asString();
        m_bank_union_no = value["bank_union_no"].asString();
        m_strRecBankName = value["bank_sub_name"].asString();
        m_strRequestTime = value["request_time"].asString();
        m_strWType = value["withdraw_type"].asString();
        m_strWithdrawDate = value["withdraw_date"].asString();

        m_strSign = value["sign"].asString();
        m_strMchntNo = value["organize_no"].asString();

        m_strChannelCode = value["channel_id"].asString();

        m_nAmount = atoi(m_strAmount.c_str());
    }
}

void PropertyPayment::printRequset()
{
    std::cout << "-------------parseJson:------------" << std::endl;
    std::cout << "version=" << m_strVersion << std::endl;
    std::cout << "request_id=" << m_strRequestId << std::endl;
    std::cout << "organize_code=" << m_strOrganizeCode << std::endl;
    std::cout << "amount=" << m_strAmount << std::endl;
    std::cout << "receive_card_id=" << m_strRecCardId << std::endl;
    std::cout << "receive_user_name=" << m_strRecUserName << std::endl;
    std::cout << "receive_card_bank=" << m_strRecCardBank << std::endl;
    std::cout << "receive_card_sub_bank=" << m_strRecCardSubBank << std::endl;
    std::cout << "request_time=" << m_strRequestTime << std::endl;
    std::cout << "wtype=" << m_strWType << std::endl;
    std::cout << "memo=" << m_strMemo << std::endl;
    std::cout << "sign=" << m_strSign << std::endl;
    std::cout << "------------------------------------" << std::endl;
}

void PropertyPayment::makeJson(std::string &strOutput)
{
    #ifdef DEBUG
        printResponse();
    #endif // DEBUG
    Json::Value root;

    root["ret_code"] = StringUtility::IntToString(m_nRetCode);
    root["ret_msg"] = m_strRetMsg;

   // strOutput = root.toStyledString();

    Json::FastWriter writer;
	strOutput = writer.write(root);
}

void PropertyPayment::printResponse()
{
    std::cout << "-------------makeJson:------------" << std::endl;
    std::cout << "payAccId=" << m_strPayAccId << std::endl;
    std::cout << "ret_code=" << m_nRetCode << std::endl;
    std::cout << "ret_msg=" << m_strRetMsg << std::endl;
    std::cout << "------------------------------------" << std::endl;
}
