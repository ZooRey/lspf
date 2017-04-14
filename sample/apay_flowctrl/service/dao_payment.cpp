#include "dao_payment.h"
#include "property_payment.h"
#include "database/db_oracle.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include "log/log.h"
#include "common/string_utility.h"

DAOPayment::DAOPayment(const int nId):m_db_cmd(new DBCommand(nId))
{
    std::cout << "DAOPayment(const int nId)" <<std::endl;
}

DAOPayment::DAOPayment(DBCommandPtr db_cmd) : m_db_cmd(db_cmd)
{
    std::cout << "DAOPayment(DBCommandPtr db_cmd)" <<std::endl;
}

DAOPayment::~DAOPayment()
{
    std::cout << "~DAOPayment()" <<std::endl;
}

//获取公钥
bool DAOPayment::getRsaPubKey(const std::string strMchntNo, std::string &strPubKey, std::string &strMchntName)
{
	std::string strSql;

    try{
        PLOG_DEBUG("getRsaPubKey begin");
        strSql = std::string("select mer_name, rsa_public_key from nac_user.t_onl_merchant where mer_id='") + strMchntNo + "'";
        ResultSet *pRs = m_db_cmd->executeQuery(strSql);
		if (!pRs->next())
		{
			PLOG_ERROR("getRsaPubKey:%s\n", strSql.c_str());
			return false;
		}

        strMchntName = pRs->getString(1);
        strPubKey = pRs->getString(2);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    PLOG_DEBUG("getRsaPubKey end");

    return true;
}

//获取代付流水ID
bool DAOPayment::getPaySeqId(std::string &strPaySeqId)
{
	std::string strSql;

    try{
        std::string strSeqId;
        std::string strDate;

        strSql = std::string("select Seq_Pay_Acc.nextval as seqid, to_char(sysdate, 'YYYYMMDD') as tdate from dual");

        ResultSet *pRs = m_db_cmd->executeQuery(strSql);

		if (!pRs->next())
		{
			PLOG_ERROR("getRsaPubKey:%s\n", strSql.c_str());
			return false;
		}

        strSeqId = pRs->getString(1);
        strDate = pRs->getString(2);

        strPaySeqId = strDate + string(10-strSeqId.size(), '0') + strSeqId;

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}

//获取代付批次号
bool DAOPayment::getPayBatchNo(std::string &strPayBatchNo)
{
	std::string strSql;

    try{
        std::string strDate;
        std::string strSeqId;

        strSql = std::string("select Seq_CITIC_Bat_NO.nextval as seqid, to_char(sysdate, 'YYYYMMDD') as tdate from dual");

        ResultSet *pRs = m_db_cmd->executeQuery(strSql);

		if (!pRs->next())
		{
			PLOG_ERROR("getPayBatchNo:%s\n", strSql.c_str());
			return false;
		}

        strSeqId = pRs->getString(1);
        strDate = pRs->getString(2);

        strPayBatchNo = strDate.substr(2) + string(8-strSeqId.size(), '0') + strSeqId;

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}

//设置代付业务类型代码
bool DAOPayment::setTxnCode(PropertyPayment *pProperty)
{
	std::string strSql;

    try{
        std::string strState;

        strSql = std::string("select state from T_ACC_WITHDRAW_LOG where WITHDRAW_ID='") + pProperty->getRequestId() + "'";

        ResultSet *pRs = m_db_cmd->executeQuery(strSql);

		if (!pRs->next()){
			PLOG_ERROR("getPayBatchNo:%s\n", strSql.c_str());
			return false;
		}

        strState = pRs->getString(1);
        if (strState == "6"){
            pProperty->setTxnCode("T1");
        }else{
            pProperty->setTxnCode("D0");
        }

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}

//检查是否自动结算查询 只查询自动标志
int DAOPayment::autoSettMerch(const string &strMerchNo)
{
	std::string strSql;

	try{
        std::string strSettType;

        strSql = std::string("select sett_type  from nac_user.t_onl_merchant  where state='1' and mer_id='") + strMerchNo + "'";

        ResultSet *pRs = m_db_cmd->executeQuery(strSql);

		if (!pRs->next()){
			PLOG_ERROR("getPayBatchNo:%s\n", strSql.c_str());
			return false;
		}

        strSettType = pRs->getString(1);

        if (strSettType != "1"){
            return 1;
        }
    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}
    return 0;
}


//插入代付请求流水表（原始请求流水）
bool DAOPayment::insertPayACC(const PropertyPayment *pProperty)
{
	std::string strSql;

    try{
        std::string strWithdrawCode;

		Statement *statement = m_db_cmd->getStatement();

        strSql.assign("insert into EV_PAY_ACC \
                      (PAY_RQS_ID,      BAT_RQS_ID,      CUST_ID,           MCH_NO,         RCV_ACCT_NO, \
                       ACCT_NAM,        CCY_CD,         TRX_AMT,            WTD_PRD_CD,     BUS_ENC, \
                       BUS_BIG_CLS_CD,  BUS_SML_CLS_CD,  PAY_CHL_ID,        RQS_DT_TM, \
                       RQS_SRC_TYP_CD,  CRT_USR_ID,      LAST_UPDT_USR_ID,  CRT_DT, CRT_TM, LAST_UPDT_DT_TM, STATUS, RET_CODE,RET_MSG ) \
                       values \
                      (:1, :2, :3, :4, :5, :6, :7, :8, :9, :10, :11,  :12,  :13, sysdate, :14, :15, :16, \
                         trunc(sysdate), to_char(sysdate, 'HH24MISS'), sysdate, '0', '03', 'Initialize the state.')");

		statement->setSQL(strSql);

        statement->setString(1, pProperty->getRequestId());
        statement->setString(2, pProperty->getBatchNo());
        statement->setString(3, "");
        statement->setString(4, pProperty->getMchntNo());
        statement->setString(5, pProperty->getRecCardId());
        statement->setString(6, pProperty->getRecUserName());
        statement->setString(7, "156");
        statement->setInt(8, pProperty->getAmount());

        if (pProperty->getWType() == "1"){
            strWithdrawCode = "10";
        }else if (pProperty->getWType() == "0"){
            strWithdrawCode = "01";
        }else{
            strWithdrawCode = "unkown";
        }
        statement->setString(9, strWithdrawCode);
        statement->setString(10, "");
        statement->setString(11, "");
        statement->setString(12, "");
        statement->setString(13, pProperty->getChannelCode());
        statement->setString(14, "");
        statement->setString(15, "accp_payment");
        statement->setString(16, "accp_payment");

        statement->setAutoCommit(true);
        if (statement->executeUpdate() <= 0){
            PLOG_ERROR("insertPayACC error: PAY_RQS_ID=%s, RCV_ACCT_NO=%s, ACCT_NAM=%s, TRX_AMT=%d, WTD_PRD_CD=%s ",
                       pProperty->getRequestId().c_str(), pProperty->getRecCardId().c_str(), pProperty->getRecUserName().c_str(),
                       pProperty->getAmount(),strWithdrawCode.c_str());

            return false;
        }
    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}


//更新代付请求结果流水表结果代码（原始请求流水）
bool DAOPayment::updatePayACC(const std::string &strRequestId, const int status, const std::string &ret_code, const std::string &ret_msg )
{
	std::string strSql;
	try{
		Statement *statement = m_db_cmd->getStatement();

        strSql.assign("update EV_PAY_ACC set status=:1, ret_code=:2 ,ret_msg=:3, LAST_UPDT_DT_TM = sysdate where PAY_RQS_ID = :4");

        statement->setSQL(strSql);

		statement->setString(1, StringUtility::IntToString(status));
        statement->setString(2, ret_code);
        statement->setString(3, ret_msg);
        statement->setString(4, strRequestId);


        statement->setAutoCommit(true);
        if (statement->executeUpdate() <= 0){
            PLOG_ERROR("updatePayACC error: status=%d,ret_code=%s,strRequestId=%s ", status,ret_code.c_str(), strRequestId.c_str());
            return false;
        }
    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}

//插入代付请求流水表
bool DAOPayment::insertPayDTL(const PropertyPayment *pProperty, const PayRecord *pRecord)
{
	std::string strSql;
    try{

        std::string strWithdrawCode;
        if (pProperty->getWType() == "1"){
            strWithdrawCode = "10";
        }else if (pProperty->getWType() == "0"){
            strWithdrawCode = "01";
        }else{
            strWithdrawCode = "unkown";
        }

		Statement *statement = m_db_cmd->getStatement();

        strSql.assign("insert into EV_PAY_DTL \
                      (PAY_ACC_ID,   PAY_RQS_ID, CUST_ID,  PAY_CHL_ID,       ACC_BAT_NO,       \
                       Wtd_Prd_Cd ,   CCY_CD, TRX_AMT,      PAY_ACCT_NO,      RCV_ACCT_NO,         \
                       RCV_ACCT_NAM, RCV_ACCT_OPN_BANK_NO, RCV_ACCT_OPN_BANK_NAM,     \
                       PAY_DT,       RUT_RUL_ID,           RUT_DEAL_DT_TM,   SYS_CMSN_FEE,              \
                       SYS_CMSN_FEE_CLC_STS_CD,            SYS_CMSN_FEE_CLC_TM,                  \
                       MCH_NO,       MCH_NAM,              RCV_ACCT_TYP_CD,  AUD_STS_CD,   SSP_NOT_PAY_FLG, \
                       CRT_USR_ID,   LAST_UPDT_USR_ID,     CRT_DT, CRT_TM,   LAST_UPDT_DT_TM, STATUS, RET_CODE,RET_MSG) \
                       values \
                      (:1, :2, :3, :4,   :5, :6, :7, :8, :9, :10, :11, :12, :13,     \
                       to_date(:14, 'YYYYMMDD'), :15, sysdate, :16,              \
                       :17, :18,  :19, :20, :21,  :22,   :23, :24,   :25,   \
                       trunc(sysdate), to_char(sysdate, 'HH24MISS'), sysdate, '0', '03', 'Initialize the state.' )");

        statement->setSQL(strSql);

        statement->setString(1, pRecord->getPayId());
        statement->setString(2, pProperty->getRequestId());
        statement->setString(3, "");
        statement->setString(4, pProperty->getChannelCode());
        statement->setString(5, pProperty->getBatchNo());
        statement->setString(6, strWithdrawCode);
        statement->setString(7, "156");
        statement->setInt(8, pRecord->getPayAmt());
        statement->setString(9, "");
        statement->setString(10, pProperty->getRecCardId());
        statement->setString(11, pProperty->getRecUserName());
        statement->setString(12, pProperty->getRecCardBank());
        statement->setString(13, pProperty->getRecBankName());
        statement->setString(14, pProperty->getPayDate());
        statement->setString(15, pProperty->getRouteRuleId());
        statement->setInt(16, 0);
        statement->setString(17, "");
        statement->setString(18, "");
        statement->setString(19, pProperty->getMchntNo());
        statement->setString(20, pProperty->getMchntName());
        statement->setString(21, "");
        statement->setString(22, "");
        statement->setString(23, "N");
        statement->setString(24, "accp_payment");
        statement->setString(25, "accp_payment");

        statement->setAutoCommit(true);

        if (statement->executeUpdate() <= 0){
            PLOG_ERROR("insertPayDTL error: PAY_RQS_ID=%s, RCV_ACCT_NO=%s, ACCT_NAM=%s, TRX_AMT=%d, WTD_PRD_CD=%s ",
                pProperty->getRequestId().c_str(), pProperty->getRecCardId().c_str(), pProperty->getRecUserName().c_str(),
                pProperty->getAmount(), strWithdrawCode.c_str());

            return false;
        }

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}


//更新所有代付受理结果流水表结果代码（拆分后流水）
bool DAOPayment::updatePayDTL(const std::string &pay_acc_id,  const int status, const std::string &ret_code, const std::string &ret_msg)
{
	std::string strSql;
    try{

        Statement *statement = m_db_cmd->getStatement();

         strSql.assign("update EV_PAY_DTL set STATUS=:1, RET_CODE=:2,RET_MSG=:3,LAST_UPDT_DT_TM = sysdate where PAY_ACC_ID=:4");
        statement->setSQL(strSql);

        statement->setString(1, StringUtility::IntToString(status));
        statement->setString(2, ret_code);
        statement->setString(3, ret_msg);
        statement->setString(4, pay_acc_id);

        statement->setAutoCommit(true);
        if (statement->executeUpdate() <= 0){
            PLOG_ERROR("update EV_PAY_DTL pay_acc_id=[%s], status=[%d], ret_code=[%s]", pay_acc_id.c_str(), status, ret_code.c_str());
            return false;
        }

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}

//获取商户余额
bool DAOPayment::getBalance(const string &strMerNo, string &strBalance)
{
	std::string strSql;
    try{
        std::string strMchntName;
        Statement *statement = m_db_cmd->getStatement();


        strSql.assign("select b.cons_ac_bal as balance from t_acc_bal b where b.pay_ac_no = (select a.pay_ac_no "
                    "from t_acc_profile a where a.protocol_id = (select protocol_id from crm_user.t_business t "
                    "where t.customer_id = (select distinct (customer_id) from nac_user.t_mer_busi "
                    "where merchno = :1) and t.business_type1 = '3001') "
                    "and a.mer_id = :1 and a.gl_code = '22028001')");

        statement->setSQL(strSql);
        statement->setString(1, strMerNo);

        ResultSet *pRs = statement->executeQuery();

		if (!pRs->next() ){
            ///必须管理结果集，否则内存泄漏
            statement->closeResultSet(pRs);
			PLOG_ERROR("getBalance:%s\n", strSql.c_str());
			return false;
		}

        strBalance = pRs->getString(1);

        ///必须管理结果集，否则内存泄漏
        statement->closeResultSet(pRs);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}
/*
//获取代付请求付款状态
int DAOPayment::getPaymentStatus(const string &strRequestId, std::string &strTime, int &nTotalAmt)
{
    int iRet = 5;
	std::string strSql;
    try{
        strSql = std::string("select a.TRX_AMT as payamt, to_char(a.Last_Updt_Dt_Tm, 'YYYY-MM-DD HH24:MI:SS') as datetime, \
                       status from EV_PAY_ACC a  where a.PAY_RQS_ID = '") + strRequestId + "'";

        ResultSet *pRs = m_db_cmd->executeQuery(strSql);

		if (!pRs->next()){
			PLOG_ERROR("getPaymentStatus:%s\n", strSql.c_str());
			return 1;
		}

        std::string strStatus;

        nTotalAmt = pRs->getInt(1);
        strTime = pRs->getString(2);
        strStatus = pRs->getString(3);

        int nStatus = atoi(strStatus.c_str());

        //应答代码转换
        if (nStatus == 2){
            //提现失败
            iRet = 4;
        }else if (nStatus == 1){
            //全部成功
            iRet = 0;
        }else{
            iRet = 5;
        }
    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return iRet;
}
*/
//获取代付请求付款状态
int DAOPayment::getPaymentStatus(const string &strRequestId, std::string &strTime, int &nTotalAmt,std::string &ret_msg)
{
    int iRet = 5;
	std::string sql;
    try{
        sql = std::string("select a.TRX_AMT as payamt, to_char(a.Last_Updt_Dt_Tm, 'YYYY-MM-DD HH24:MI:SS') as datetime, \
                       status,ret_msg from EV_PAY_ACC a  where a.PAY_RQS_ID = '") + strRequestId + "'";

		Statement *statement = m_db_cmd->getStatement();


/*
		statement->setSQL(sql);
        ResultSet *rs = statement->executeQuery();

		if (!rs->next()){
            statement->closeResultSet(rs);
			PLOG_ERROR("getPaymentStatus:%s\n", sql.c_str());
			return 1;
		}

        std::string strStatus;

        nTotalAmt = rs->getInt(1);
        strTime = rs->getString(2);
        strStatus = rs->getString(3);
        ret_msg = rs->getString(4);

        int nStatus = atoi(strStatus.c_str());

        //应答代码转换
        if (nStatus == 2){
            //提现失败
            iRet = 4;
        }else if (nStatus == 1){
            //全部成功
            iRet = 0;
        }else{
            iRet = 5;
        }

        statement->closeResultSet(rs);
*/
    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), sql.c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return iRet;
}

//获取原代付请求信息，返回0成功 -1错误，1记录不存在
int DAOPayment::getPayACC(const string &strRequestId, std::string &strBatchNo, std::string &strAccountNo, std::string &strChannelId)
{
	std::string strSql;

    try{
        strSql = std::string("select BAT_RQS_ID, RCV_ACCT_NO, PAY_CHL_ID from EV_PAY_ACC a where a.PAY_RQS_ID =  '") + strRequestId + "'";

        ResultSet *pRs = m_db_cmd->executeQuery(strSql);
		if (!pRs->next()){
            PLOG_INFO("getPayACC: record is not existed:");
			return 1;
		}

        strBatchNo = pRs->getString(1);
        strAccountNo = pRs->getString(2);
        strChannelId = pRs->getString(3);


    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return 0;
}


//获取代付受理ID（已拆分后的记录）
bool DAOPayment::getPayDTL(const string &strRequestId, std::vector<PayRecord *> &vecPayAccId)
{
	std::string strSql;
    try{
        int nAmount;
        std::string strErrCode;
        std::string strPayAccId;

        strSql = std::string(" select PAY_ACC_ID, trx_amt, status from EV_PAY_DTL where PAY_RQS_ID = '") + strRequestId + "'";

        ResultSet *pRs = m_db_cmd->executeQuery(strSql);

        while (pRs->next())
        {
            PayRecord *pRecord = new PayRecord;
			strPayAccId = pRs->getString(1);
            nAmount = pRs->getInt(2);
            strErrCode = pRs->getString(3);


            pRecord->setPayAmt(nAmount);
            pRecord->setErrCode(strErrCode);
            pRecord->setPayId(strPayAccId);

            vecPayAccId.push_back(pRecord);
        }

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}
    return true;
}
//获取代付明细状态
//返回2成功，1银行处理中，3失败
int DAOPayment::getPayDtlStatus(const string &strPayAccId)
{
	std::string strSql;
    std::string strPayStatus;

    try{
        strSql = std::string("select status  from EV_PAY_DTL where PAY_ACC_ID='") + strPayAccId + "'";

        ResultSet *pRs = m_db_cmd->executeQuery(strSql);

		if (!pRs->next()){
			PLOG_ERROR("no record :%s\n", strSql.c_str());
			return 4;
		}

        strPayStatus = pRs->getString(1);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return atoi(strPayStatus.c_str());
}

//获取卡bin信息
bool DAOPayment::getCardBin(const std::string &strCardNo, std::string &strBankCode, std::string &strBankName)
{
	std::string strSql;
    try{
		Statement *statement = m_db_cmd->getStatement();

        strSql.assign("select insuer_id,insuer_name from crm_user.t_bankcard_bin m \
					   where m.PAN_LENGTH = length(:1) and m.BIN_CTT =substr(:1, 1, m.BIN_LENGTH)");


		statement->setSQL(strSql);

		statement->setString(1, strCardNo);

        ResultSet *pRs = statement->executeQuery();

		if (!pRs->next()){
            ///必须管理结果集，否则内存泄漏
            statement->closeResultSet(pRs);
			PLOG_ERROR("no record :%s\n", strSql.c_str());
			return false;
		}

        strBankCode = pRs->getString(1);
        strBankName = pRs->getString(2);
        ///必须管理结果集，否则内存泄漏
        statement->closeResultSet(pRs);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}

//获取卡bin信息
bool DAOPayment::getCardBin(const std::string &strCardNo, std::string &strBankCode, std::string &strBankName, std::string &strBankUnionNo)
{
	std::string strSql;
    try{
		Statement *statement = m_db_cmd->getStatement();

        strSql.assign("select b.insuer_name, b.insuer_id, a.sub_bank_code "
                          "from crm_user.t_bank a, crm_user.t_bankcard_bin b "
                         "where a.bank_code = to_number(b.bankcode) "
                           "and b.pan_length = length(:1) "
                           "and b.bin_ctt = substr(:1, 1, b.bin_length)");


		statement->setSQL(strSql);

		statement->setString(1, strCardNo);

        ResultSet *pRs = statement->executeQuery();

		if (!pRs->next()){
                ///必须管理结果集，否则内存泄漏
            statement->closeResultSet(pRs);
			PLOG_ERROR("no record :%s\n", strSql.c_str());
			return false;
		}

        strBankName     = pRs->getString(1);
        strBankCode     = pRs->getString(2);
        strBankUnionNo  = pRs->getString(3);
        ///必须管理结果集，否则内存泄漏
        statement->closeResultSet(pRs);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;
}




//获取付款日期
bool DAOPayment::getPayDate(std::string &strPayDate, const std::string &strWithdrawDate, const std::string &strWType)
{
	std::string strSql;
    try{
		Statement *statement = m_db_cmd->getStatement();

        if (strWType != "0"){
            //非D0需查询下一个工作日
            strSql.assign("select to_char(min(t1.tdate), 'YYYYMMDD') as workdate from "
                          " ( select to_date(:1,'YYYYMMDD') + level as tdate from dual connect by level < 10) t1"
                          " left join acc_user.T_HOLIDAY  t2 "
                          " on to_char(t1.tdate, 'YYYYMMDD') = t2.holiday "
                          " where t2.property is null");
        }else{
            //D0需查询当前日期
            strSql.assign("select to_char(sysdate,'YYYYMMDD') as workdate from dual");
        }

        statement->setSQL(strSql);
        if (strWType != "0")
        {
			statement->setString(1, strWithdrawDate);
        }

        ResultSet *pRs = statement->executeQuery();

		if (!pRs->next()){
            ///必须管理结果集，否则内存泄漏
            statement->closeResultSet(pRs);
			PLOG_ERROR("no record :%s\n", strSql.c_str());
			return false;
		}
        strPayDate = pRs->getString(1);
        if (strPayDate.empty()){
            ///必须管理结果集，否则内存泄漏
            statement->closeResultSet(pRs);
            PLOG_ERROR("select error:paydate is empty()!!!");
            return false;
        }

        ///必须管理结果集，否则内存泄漏
        statement->closeResultSet(pRs);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), strSql.c_str());
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return true;

}

//代付信息入库
int DAOPayment::InsertIntoCitic(const std::string &pay_id, const PropertyPayment *property)
{
    char sql[1024] = {0};
    try
    {

        sprintf(sql, "insert into acc_user.if_citic_pay_dtl_itf(pay_acc_id, pack_serial_no, recaccountno, recaccountname, rcvbankcode, rcvbankname, rranamount, "
                   "bsntype, pay_status, crt_dt, last_updt_dt_tm, pay_dt) values('%s', '0', '%s', '%s', '%s', '%s', %d, '%s', %c, sysdate, sysdate, to_date('%s', 'yyyymmdd') ) ", pay_id.c_str(),
                   property->getRecCardId().c_str(), property->getRecUserName().c_str(),
                   property->getRecCardBank().c_str(), property->getRecBankName().c_str(), property->getAmount(), "09001", '0', property->getPayDate().c_str());

        int rc = m_db_cmd->executeUpdate(sql);
        if(rc != 0){
            PLOG_ERROR("InsertIntoCitic failed: sql=[%s]\n", sql);
            return -1;
        }
    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), sql);
        return false;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return false;
	}

    return 0;
}

