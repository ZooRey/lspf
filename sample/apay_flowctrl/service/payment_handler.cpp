#include "payment_handler.h"
#include "dao_payment.h"
#include "property_payment.h"
#include "data_sign.h"
#include "app_exception.h"
#include "oldapi/xsvc_client.h"
#include "route/route_calculator.h"
#include "route/route_dispath.h"
#include "riskctrl/risk_control.h"

#include <json/json.h>
#include <iostream>

#include "log/log.h"
#include "common/app_conf.h"
#include "common/time_utility.h"

PaymentHandler::PaymentHandler() :  m_bInsertAcc(false),
                                    m_bInsertDtl(false),
                                    m_status(0),
                                    m_ret_code("03"),
                                    m_db_connection(new DBCommand)
{
    m_pProperty = new PropertyPayment;
    m_pDAOPayment = new DAOPayment(m_db_connection);
    m_pay_record = new PayRecord;
}

PaymentHandler::~PaymentHandler()
{
    if (m_pProperty != NULL){
        delete m_pProperty;
    }

    if (m_pDAOPayment != NULL){
        delete m_pDAOPayment;
    }

    if (m_pay_record != NULL){
        delete m_pay_record;
    }
}

int PaymentHandler::parseJson(const std::string &strInput)
{
    try{
        //解析请求报文
        m_pProperty->parseJson(strInput);

        m_logId = atoi(m_pProperty->getRequestId().c_str());

    }catch(const AppException &e){
        //设置应答错误码与错误信息
        m_pProperty->setRetCode(e.getType());
        m_pProperty->setRetMsg(e.what());

        PLOG_ERROR(e.what());
        return -1;
    }

    return 0;
}

int PaymentHandler::makeJson(std::string &strOutput)
{
    m_pProperty->makeJson(strOutput);

    return 0;
}

int PaymentHandler::process()
{
    try{
        PLOG_INFO("process start...");

        //预处理
        prepare();
        PLOG_INFO("prepare finish.");

        //检查交易风险控制
        checkRiskCtrl();
        PLOG_INFO("checkRiskCtrl finish.");

        //获取渠道路由信息
        getChlCode();
        PLOG_INFO("getChlCode finish.");

        //拆分交易金额
        split();
        PLOG_INFO("split finish.");

        //交易记账
        //account();
        //PLOG_INFO("account finish.");

        //交易稽核
        audit();
        PLOG_INFO("audit finish.");

    }catch(const AppException &e){
        //设置应答错误码与错误信息
        m_pProperty->setRetCode(e.getType());
        m_pProperty->setRetMsg(e.what());

        updateFailedResult(); //更新交易失败结果
        PLOG_ERROR("AppException:%s", e.what());
        return 0;
    }

    try{
        if (m_pProperty->getChannelCode() == "03020000"){
            RouteToCitic();//T+1代付交易直接插入中信渠道表
        }else{
            PLOG_INFO("route start.");
            route(); //交易渠道路由
            PLOG_INFO("route finish.");
        }
    }catch(const AppException &e){
        PLOG_ERROR("AppException:%s", e.what());
        return 0;
    }

    PLOG_INFO("process finish.");

    return 0;
}

//校验签名
int PaymentHandler::verifySign()
{
    try{
        DataSign data_sign;
        PLOG_DEBUG("verifySign begin...");

        if (!data_sign.GetEncKey(m_pProperty->getOrganizeCode())){
            std::string strPubKey;
            std::string strMchntName;

            PLOG_DEBUG("getRsaPubKey begin...");
            //从数据库获取公钥
            if (!(m_pDAOPayment->getRsaPubKey(m_pProperty->getOrganizeCode(),strPubKey, strMchntName)))
            {
                PLOG_ERROR("error:getRsaPubKey failed");
                throw AppException(AppException::INTERNAL_ERROR);
            }
            PLOG_DEBUG("getRsaPubKey succeed...");

            m_pProperty->setMchntName(strMchntName);
            data_sign.SetEncKey(strPubKey);
        }

        data_sign.addItem("version", m_pProperty->getVersion());
        data_sign.addItem("request_id", m_pProperty->getRequestId());
        data_sign.addItem("organize_no", m_pProperty->getOrganizeCode());
        data_sign.addItem("amount", m_pProperty->getStringAmount());
        data_sign.addItem("account_no", m_pProperty->getRecCardId());
        data_sign.addItem("account_name", m_pProperty->getRecUserName());
        data_sign.addItem("account_type", m_pProperty->getAccountType());
        data_sign.addItem("bank_no", m_pProperty->getRecCardBank());
        //data_sign.AddItem("bank_name", m_acc_bankname);
        data_sign.addItem("bank_union_no", m_pProperty->getBankUnionNo());
        data_sign.addItem("request_time", m_pProperty->getRequestTime());
        data_sign.addItem("withdraw_date", m_pProperty->getWithdrawDate());
        data_sign.addItem("withdraw_type", m_pProperty->getWType());

        if (!(data_sign.verify(m_pProperty->getSign())))
        {
            PLOG_ERROR("error:verify data sign failed");
            throw AppException(AppException::INVALID_SIGN);
        }
    }catch(const AppException &e){
        //设置应答错误码与错误信息
        m_pProperty->setRetCode(e.getType());
        m_pProperty->setRetMsg(e.what());

        PLOG_ERROR("AppException:%s", e.what());
        return -1;
    }

    PLOG_DEBUG("verifySign end...");

    return 0;
}

//数据预处理
void PaymentHandler::prepare()
{

    if (m_pProperty->getAccountType() == "1"){
        if (m_pProperty->getBankUnionNo().empty() || m_pProperty->getRecBankName().empty() ){
            PLOG_ERROR("company's account error:bank union no or bank sub name is empty..");
            throw AppException(AppException::PAYMENT_FAILED);
        }
    }else{
        //非对公账号获取银行行号和名称
        std::string strBankCode;
        std::string strBankName;
        std::string strBankUnionNo;

        if (!(m_pDAOPayment->getCardBin(m_pProperty->getRecCardId(), strBankCode, strBankName, strBankUnionNo))){
            PLOG_ERROR("error:getCardBin failed");
            m_ret_msg.assign("cardno failed.");
            throw AppException(AppException::PAYMENT_FAILED);
        }

        m_pProperty->setRecCardBank(strBankCode);
        m_pProperty->setRecBankName(strBankName);
        m_pProperty->setBankUnionNo(strBankUnionNo);
    }

    std::string strPayBatchNo;

    //从数据库获取批次号
    if (!(m_pDAOPayment->getPayBatchNo(strPayBatchNo))){
        //获取代付交易批次号出错
        PLOG_ERROR("error:getPayBatchNo failed");
        m_ret_msg.assign("getPayBatchNo failed.");
        throw AppException(AppException::PAYMENT_FAILED);
    }

    m_pProperty->setBatchNo(strPayBatchNo);
    PLOG_DEBUG( "strPayBatchNo = [%s]", strPayBatchNo.c_str());

    std::string strPayDate;
    //获取提现的付款日期
    if (!(m_pDAOPayment->getPayDate(strPayDate, m_pProperty->getWithdrawDate(), m_pProperty->getWType()))){
        PLOG_ERROR("error:getPayDate failed");
        m_ret_msg.assign("getPayDate failed.");
        throw AppException(AppException::PAYMENT_FAILED);
    }

    m_pProperty->setPayDate(strPayDate);
    PLOG_INFO("paydate=[%s]", strPayDate.c_str());

}

int PaymentHandler::checkExist()
{
    int iRet = 0;
    std::string strBatchNo;
    std::string strAccountNo;
    std::string strChannelId;

    //获取代付请求付款信息
    iRet = m_pDAOPayment->getPayACC(m_pProperty->getRequestId(), strBatchNo, strAccountNo, strChannelId);
    if (iRet == -1)
    {
        PLOG_ERROR("error:getPayACC failed");
        throw AppException(AppException::INTERNAL_ERROR);
    }
    else if (iRet == 0)
    {
        PLOG_ERROR("The pay record is existed");
        throw AppException(AppException::RECORD_EXISTED);
    }

    return 0;
}


//检查交易风险控制
void PaymentHandler::checkRiskCtrl()
{
    RiskControl risk_control(m_pProperty->getAmount(), m_pProperty->getMchntNo(), m_pProperty->getRecCardId(), "0", m_db_connection);

    if (risk_control.CheckRisk() != 0 ){
        PLOG_ERROR("risk_control.CheckRisk failed");
        m_ret_msg.assign("risk_control.CheckRisk failed.");
        throw AppException(AppException::PAYMENT_FAILED);
    }
}

//获取渠道路由信息
void PaymentHandler::getChlCode()
{
    int rc = 0;

    std::string txn_type;
    std::string channel_id;
    std::string channel_name;

    channel_id = m_pProperty->getChannelCode();

    if (channel_id.empty()){
        std::string bank_no = m_pProperty->getRecCardBank();
        PLOG_DEBUG( "bank_no = [%s]", bank_no.c_str());

        if(!bank_no.length() >= 4){
            std::string bank_channel("channel");
            bank_channel.append(bank_no.substr(0, 4));
            channel_id = AppConf::Instance()->GetConfStr("BankChannel", bank_channel);
            txn_type = "0";
        }

        if (channel_id.empty())
        {
            RouteCalculator route_calulator(m_pProperty->getAmount(), m_pProperty->getWType(), m_pProperty->getPayDate(), m_db_connection);
            rc = route_calulator.GetChannelInfo(m_pProperty->getAccountType(), txn_type, channel_id, channel_name);
            if (rc != 0){
                PLOG_ERROR("route_calulator.GetChannelInfo failed");
                m_ret_msg.assign("GetChannelInfo failed.");

                throw AppException(AppException::PAYMENT_FAILED);
            }
        }

         //渠道代码
        m_pProperty->setChannelCode(channel_id);
        //渠道名称
        m_pProperty->setChannelName(channel_name);
        PLOG_INFO("GetChannelInfo succeed...");

    }



    PLOG_INFO("GetChannel id=[%s]...", m_pProperty->getChannelCode().c_str());

    //代付方式：0单笔，1批量
    m_pProperty->setTxnType(txn_type);

    //记录付款流水信息表
    if (!(m_pDAOPayment->insertPayACC(m_pProperty)))
    {
        PLOG_ERROR("insertPayAcc failed");
        throw AppException(AppException::PAYMENT_FAILED);
    }
    PLOG_INFO("insertPayACC succeed...");

    m_bInsertAcc = true;
}


//拆分交易金额
void PaymentHandler::split()
{
    std::string strPayId;

    //付款金额
    m_pay_record->setPayAmt(m_pProperty->getAmount());

    //从数据库获取序列ID
    if (!(m_pDAOPayment->getPaySeqId(strPayId)))
    {
        //获取代付交易受理ID出错
        PLOG_ERROR("error:call getPaySeqId failed 4");
        throw AppException(AppException::PAYMENT_FAILED);
    }
    m_pay_record->setPayId(strPayId);

    //记录付款流水信息表
    if (!(m_pDAOPayment->insertPayDTL(m_pProperty, m_pay_record)))
    {
        PLOG_ERROR("error: insertPayDTL failed");
        throw AppException(AppException::PAYMENT_FAILED);
    }

    m_bInsertDtl = true;
}


//交易记账
void PaymentHandler::account()
{
    //调用记账服务
    std::string strTime;
    std::string strInput, strOutput;

    strTime = TimeUtility::GetStringTime();

    Json::Value root;
    Json::Value arrayObj;
    Json::Value item;

    item["TXN_CHL"] = "";
    item["TXN_COD"] = "3001";
    item["TXN_SU_COD"] = "3003";
    item["TXN_DAT"] = strTime;
    item["TXN_LTL_MERCHNO"] = m_pProperty->getMchntNo();
    item["TXN_LTL_TERMNO"] = "";
    item["TXN_PAY_CHL"] = "";
    item["TXN_LTL_AMOUNT"] = m_pay_record->getPayAmt();
    item["TXN_LTL_BATCHNO"] = "";
    item["TXN_LTL_VOUCHNO"] = m_pay_record->getPayId();
    item["TXN_LTL_AUTHCODE"] = "";
    item["TXN_LTL_REFNO"] = "";
    item["TXN_LTL_FLOW"] = "";
    item["PROTOCOL_ID"] = "";

    arrayObj.append(item);

    root["SYS_API_VER"] = m_pProperty->getVersion();
    root["SYS_CALL_TOKEN"] = strTime;
    root["SYS_CALL_TIME"] = strTime;
    root["SYS_CALL_PARAM"] = arrayObj;

    //strInput = root.toStyledString();
    Json::FastWriter writer;
	strInput = writer.write(root);


//账务
#define ACC_FINANCE         (char *)"ACC_FINANCE_SERVER"        //账务系统名
#define ACC_ACCOUNTS        (char *)"ACC_SVC_ACCOUNTS"          //账务系统服务

    XSvcClient XSvcClient_(ACC_FINANCE, ACC_ACCOUNTS, m_pay_record->getPayId());
    if (!XSvcClient_.xcall(strInput, strOutput))
    {
        //调用记账出错
        PLOG_ERROR("error:call ACCP_FINANCE-ACCP_ACCOUNT service failed");
        m_ret_msg.assign("account failed.");

        throw AppException(AppException::PAYMENT_FAILED);
    }

    Json::Reader reader;
    Json::Value value;

    if (!reader.parse(strOutput, value) || value["retCode"].asInt() != 0){

        PLOG_ERROR("error:call ACCP_FINANCE-ACCP_ACCOUNT service error, %s", strOutput.c_str());
         m_ret_msg.assign("account failed.");
        throw AppException(AppException::PAYMENT_FAILED);
    }
}

//交易稽核
void PaymentHandler::audit()
{
/*
    std::vector<PayRecord*>::iterator it = m_vecPayRecord.begin();

    int nTotalAmt = 0;
    for (; it != m_vecPayRecord.end(); ++it)
    {
        nTotalAmt += (*it)->getPayAmt();
    }

    if (nTotalAmt != m_pProperty->getAmount())
    {
        //审核金额出错
        PLOG_ERROR("error:audit amount failed");
        throw AppException(AppException::PAYMENT_FAILED);
    }

    //检查是否自动结算查询 只查询自动标志
    int iRet = m_pDAOPayment->autoSettMerch(m_pProperty->getMchntNo());
    if (iRet < 0)
    {
        PLOG_ERROR("error: autoSettMerch failed");
        throw AppException(AppException::INTERNAL_ERROR);
    }
    else if (iRet > 0)
    {
        PLOG_ERROR("error: autoSettMerch is limited");
        throw AppException(AppException::PAYMENT_FAILED);
    }
*/
}

//交易路由
bool PaymentHandler::routeHandle()
{
    std::string request;
    std::string response;
    Json::Value root;

    root["pay_id"] = m_pay_record->getPayId();
    root["batch_no"] = m_pProperty->getBatchNo();
    root["channel_id"] = m_pProperty->getChannelCode();
    root["account_no"] = m_pProperty->getRecCardId();
    root["account_name"] = m_pProperty->getRecUserName();
    root["account_type"] = m_pProperty->getAccountType();

    root["bank_union_no"] = m_pProperty->getBankUnionNo();
    root["bank_code"] = m_pProperty->getRecCardBank();
    root["bank_name"] = m_pProperty->getRecBankName();
    root["amount"] = m_pay_record->getPayAmt();
    root["pay_date"] = m_pProperty->getPayDate();

    Json::FastWriter writer;
	request = writer.write(root);

    PLOG_INFO("routeHandle>>>%s", request.c_str());

    m_status = 3;
    m_ret_code.assign("02");//交易状态未知

    RouteDispath route_dispath(m_pProperty->getChannelCode());
    ///网络失败不能更新为失败，有风险！需发起状态查询交易，取最终结果...
    int rc = route_dispath.Pay(request, response);
    if (rc != 0){
        PLOG_ERROR("error:route_dispath: rc=[%d]", rc);
        m_pProperty->setRetCode(5);
        return false;
    }

    PLOG_INFO("query channel:output=[%s]", response.c_str());

    Json::Reader reader(Json::Features::strictMode());
    Json::Value value;

    if (!reader.parse(response, value)){

        PLOG_ERROR("error:call ACCP_CHANNEL-CHL_ROUTE_SVC failed: return=[%s]", response.c_str());
        m_ret_code = "02";//交易状态未知
        m_pProperty->setRetCode(5);
        return false;
    }

    m_ret_code = value["ret_code"].asString();
    m_ret_msg  = value["ret_msg"].asString();

    if (m_ret_code == "00"){   ///成功
        m_status = 1;
        m_pProperty->setRetCode(0);
        m_pProperty->setRetMsg("payment succeed");
        return true;
    }else if (m_ret_code == "01"){ ///失败
        m_status = 2;
        m_pProperty->setRetCode(4);
       // m_pProperty->setRetMsg("payment failed");
        m_pProperty->setRetMsg(m_ret_msg);
        return false;
    }else if (m_ret_code == "02"){ ///未知
        m_status = 3;
        m_pProperty->setRetCode(5);
        m_pProperty->setRetMsg("payment status is unkown");
        return true;
    }else if (m_ret_code == "03"){ ///正在处理
        m_status = 3;
        m_pProperty->setRetCode(5);
        m_pProperty->setRetMsg("payment status is unkown");
        return true;
    }else{                      ///异常
        m_status = 3;
        m_pProperty->setRetCode(5);
        m_pProperty->setRetMsg("payment status is unkown");
        return true;
    }

    return true;
}

//交易渠道路由分发
void PaymentHandler::route()
{

    routeHandle();

    //更新付款流水结果信息表
    if (!(m_pDAOPayment->updatePayACC(m_pProperty->getRequestId(), m_status, m_ret_code,m_ret_msg))){
        PLOG_ERROR("updatePayACC failed ret_code=[%s]", m_ret_code.c_str());
    }

    //更新付款流水结果信息表
    if (!(m_pDAOPayment->updatePayDTL(m_pay_record->getPayId(), m_status, m_ret_code,m_ret_msg))){
        PLOG_ERROR("updatePayDTL failed ret_code=[%s]", m_ret_code.c_str());
    }
}

void PaymentHandler::updateFailedResult()
{
    if (m_bInsertAcc){
        //更新付款流水结果信息表
        if (!(m_pDAOPayment->updatePayACC(m_pProperty->getRequestId(), 2, "01",m_ret_msg))){
            PLOG_ERROR("updatePayACC failed");
        }
    }

    if (m_bInsertDtl){
        //更新付款流水结果信息表
        if (!(m_pDAOPayment->updatePayDTL(m_pay_record->getPayId(), 2, "01",m_ret_msg))){
            PLOG_ERROR("updatePayDTL failed");
        }
    }
}

void PaymentHandler::RouteToCitic()
{
    //更新付款流水结果信息表
    if (m_pDAOPayment->InsertIntoCitic(m_pay_record->getPayId(), m_pProperty) != 0){
        PLOG_ERROR("InsertIntoCitic failed");
        m_pProperty->setRetCode(4);
        m_pProperty->setRetMsg("payment status is failed");
        updateFailedResult();
    }else{
        m_pProperty->setRetCode(5);
        m_pProperty->setRetMsg("payment is processing");
    }
}
