#include "query_handler.h"
#include "dao_payment.h"
#include "property_payment.h"
#include "data_sign.h"
#include "app_exception.h"
#include "route/route_dispath.h"
#include "oldapi/xsvc_client.h"

#include <iostream>
#include <json/json.h>
#include "log/log.h"
#include "common/time_utility.h"
#include "common/string_utility.h"

QueryHandler::QueryHandler()
{
    m_pDAOPayment = new DAOPayment(0);
}

QueryHandler::~QueryHandler()
{
    std::cout << "~QueryHandler()" <<std::endl;
    if (m_pDAOPayment != NULL){
        delete m_pDAOPayment;
    }

    if (!m_vecPayRecord.empty())
    {
        std::vector<PayRecord*>::iterator it = m_vecPayRecord.begin();
        for (; it != m_vecPayRecord.end(); ++it)
        {
            if (*it != NULL)
                delete *it;
        }
        m_vecPayRecord.clear();
    }
}

int QueryHandler::parseJson(const std::string &strInput)
{
    try{
        Json::Reader reader(Json::Features::strictMode());
        Json::Value value;

        if (reader.parse(strInput, value))
        {
            m_strVersion = value["version"].asString();
            m_strRequestId = value["request_id"].asString();
            m_strOrganizeCode = value["organize_no"].asString();

            m_strSign = value["sign"].asString();
        }
    }catch(AppException &e){
        //设置应答错误码与错误信息
        m_nRetCode = static_cast<int>(e.getType());
        m_strRetMsg.assign(e.what());

        PLOG_ERROR("AppException:%s", e.what());
        return -1;
    }

    return 0;
}

int QueryHandler::makeJson(std::string &strOutput)
{
    Json::Value root;

    root["ret_code"] = StringUtility::IntToString(m_nRetCode);;
    root["ret_msg"] = m_strRetMsg;

  //  strOutput = root.toStyledString();
    Json::FastWriter writer;
	strOutput = writer.write(root);

    return 0;
}

int QueryHandler::process()
{
    PLOG_DEBUG("QueryHandler::process() start...");
    try{

        //本地数据表查询
        m_nRetCode = localQuery();

        if (m_nRetCode < 0)
        {
            PLOG_INFO("channelQuery() start...");
            m_nRetCode = channelQuery();
            PLOG_INFO("channelQuery() end ret=[%d]", m_nRetCode);
        }

    }catch(AppException &e){
        //设置应答错误码与错误信息

        m_nRetCode = static_cast<int>(e.getType());
        m_strRetMsg.assign(e.what());

        PLOG_ERROR("AppException:%s", e.what());
        return -1;
    }

    PLOG_DEBUG("QueryHandler::process() end...");

    return 0;
}

//校验签名
int QueryHandler::verifySign()
{
    try{
        DataSign data_sign;
        PLOG_DEBUG("verifySign begin...");

        if (!data_sign.GetEncKey(m_strOrganizeCode)){
            std::string strPubKey;
            std::string strMchntName;
            //从数据库获取公钥
            if (!(m_pDAOPayment->getRsaPubKey(m_strOrganizeCode,strPubKey, strMchntName)))
            {
                PLOG_ERROR("error:getRsaPubKey failed");
                throw AppException(AppException::INTERNAL_ERROR);
            }
            PLOG_INFO("getRsaPubKey succeed...");

            data_sign.SetEncKey(strPubKey);
        }

        data_sign.addItem("version", m_strVersion);
        data_sign.addItem("request_id", m_strRequestId);
        data_sign.addItem("organize_no", m_strOrganizeCode);

        if (!(data_sign.verify(m_strSign)))
        {
            PLOG_ERROR( "error:verify data sign failed");
            throw AppException(AppException::INVALID_SIGN);
        }
    }catch(AppException &e){
        //设置应答错误码与错误信息
        m_nRetCode = static_cast<int>(e.getType());
        m_strRetMsg.assign(e.what());

        PLOG_ERROR("AppException:%s", e.what());
        return -1;
    }
    return 0;
}

//本地数据表查询结果,返回true为确定查询结果，false则还需后续处理
int QueryHandler::localQuery()
{
    int iRet = 5;
    std::string strTime;
    std::string ret_msg;
    //获取代付请求状态
    iRet = m_pDAOPayment->getPaymentStatus(m_strRequestId, strTime, m_nTotalAmt,ret_msg);
    if (iRet < 0)
    {
        //查询出错，返回状态码为5，银行处理中
        return 5;
    }

    if (iRet == 5){
        std::string strNowTime = TimeUtility::GetStringTime("-");
        if (TimeUtility::GetTimeDiff(strNowTime, strTime) > 180){
            //超过3分钟的交易需要查询渠道交易结果
            return -1;
        }
    }

    m_strRetMsg = ret_msg;


    return iRet;
}

//通过渠道查询结果
int QueryHandler::channelQuery()
{
    if (!(m_pDAOPayment->getPayDTL(m_strRequestId, m_vecPayRecord)))
    {
        //查询出错，返回状态码为5，银行处理中
        return 5;
    }

    if (m_vecPayRecord.size() == 0){
        PLOG_INFO("getPayDTL error: no record");
        return 4;
    }

    int iRet = 0;
    std::string strBatchNo;
    std::string strAccountNo;
    std::string strChannelId;

    //获取代付受理ID
    if ((m_pDAOPayment->getPayACC(m_strRequestId, strBatchNo, strAccountNo, strChannelId) != 0))
    {
        //查询出错，返回状态码为5，银行处理中
        return 5;
    }

    PLOG_INFO("call channelQueryHandle request_id=[%s] start...", m_strRequestId.c_str());

    std::vector<PayRecord*>::iterator it = m_vecPayRecord.begin();
    for (; it != m_vecPayRecord.end(); ++it)
    {
        PLOG_INFO("call channelQueryHandle pay_id=[%s]", (*it)->getPayId().c_str());
        //状态为处理中，查询渠道交易结果
        iRet = channelQueryHandle((*it)->getPayId(), strBatchNo, strAccountNo, strChannelId);
        if (iRet == 1){
            //成功
            return 0;
        }else if (iRet == 2){
            //失败
            return 4;
        }else{
            //未得到结果或者是结果返回5
            return 5;
        }
    }

    PLOG_INFO("call channelQueryHandle request_id=[%s] end...", m_strRequestId.c_str());

    return iRet;
}

//交易路由,返回1处理中，2成功，3失败, 4无记录
int QueryHandler::channelQueryHandle(const std::string &strPayId, const std::string &strBatchNo,
                                     const std::string &strAccountNo, const std::string &strChannelId)
{
    int status = 0;
    std::string request;
    std::string response;

    Json::Value root;

    root["pay_id"] = strPayId;
    root["batch_no"] = strBatchNo;
    root["channel_id"] = strChannelId;
    root["account_no"] = strAccountNo;

    //request = root.toStyledString();

    Json::FastWriter writer;
	request = writer.write(root);


    PLOG_INFO("query channel:input=[%s], channel_id=[%s]", request.c_str(), strChannelId.c_str());

    RouteDispath route_dispath(strChannelId);

    int rc = route_dispath.Query(request, response);

    if (rc != 0){
        PLOG_ERROR("error:route_dispath: rc=[%d]", rc);

        return 5;
    }

    PLOG_INFO("query channel:output=[%s]", response.c_str());

    Json::Reader reader(Json::Features::strictMode());
    Json::Value value;
    if (!reader.parse(response, value)){

        PLOG_ERROR("error:call ACCP_CHANNEL-CHL_ROUTE_SVC failed: return=[%s]", response.c_str());
        return 5;
    }

    std::string ret_code = value["ret_code"].asString();

    if (ret_code != "00"){   ///查询失败
        PLOG_INFO("call channel query failed  pay_id=[%s]", strPayId.c_str());
        return -1;
    }

    std::string ori_ret_code = value["ori_ret_code"].asString();
    std::string ori_ret_msg  = value["ori_ret_msg"].asString();
    if (ori_ret_code == "00"){ ///成功
        status = 1;
    }else if (ori_ret_code == "01"){ ///失败
        status = 2;
    }else if (ori_ret_code == "02"){ ///未知
        status = 3;
    }


    PLOG_INFO("call channel query ori_ret_code=[%s], status=[%d]", ori_ret_code.c_str(), status);

   //更新查询结果
    if (!m_pDAOPayment->updatePayDTL(strPayId, status, ori_ret_code,ori_ret_msg)){
        //更新出错
        PLOG_ERROR("updatePayDTL error");
        return 5;
    }

    //更新查询结果
    if (!m_pDAOPayment->updatePayACC(m_strRequestId, status, ori_ret_code,ori_ret_msg)){
        //更新出错
        PLOG_ERROR("updatePayACC error");
        return 5;
    }

    m_strRetMsg = ori_ret_msg;

    return status;
}
