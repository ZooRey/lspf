#include "dao_riskctrl.h"
#include <iostream>
#include <stdio.h>
#include "log/log.h"
#include "database/db_oracle.h"

DAORiskCtrl::DAORiskCtrl(const int nId):m_db_cmd(new DBCommand(nId))
{

}

DAORiskCtrl::DAORiskCtrl(DBCommandPtr db_cmd):m_db_cmd(db_cmd)
{

}

DAORiskCtrl::~DAORiskCtrl()
{

}

//检查卡bin
int DAORiskCtrl::CheckCardBin(const std::string &strCardNo)
{
    try{
        std::string sql;
		Statement *statement = m_db_cmd->getStatement();

        sql.assign("select count(1) as num from crm_user.t_bankcard_bin m  \
                   where m.PAN_LENGTH = length(:1) and m.BIN_CTT =substr(:1, 1, m.BIN_LENGTH)");
        statement->setSQL(sql);

        statement->setString(1, strCardNo);

        ResultSet *rs = statement->executeQuery();
        if (!rs->next() || rs->getInt(1) == 0){
            ///必须管理结果集，否则内存泄漏
            statement->closeResultSet(rs);
            PLOG_ERROR("CheckCardBin error: cardno=%s", strCardNo.c_str());
            return 1;
        }

        ///必须管理结果集，否则内存泄漏
        statement->closeResultSet(rs);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return 0;
}



//检查是否自动结算查询 只查询自动标志
int DAORiskCtrl::AutoSettMerch(const std::string &strMerchNo)
{
    try{
         std::string sql;
		Statement *statement = m_db_cmd->getStatement();

        sql.assign("select sett_type from nac_user.t_onl_merchant  where state='1' and mer_id=:1");

        statement->setSQL(sql);

        statement->setString(1, strMerchNo);


        ResultSet *rs = statement->executeQuery();
        if (!rs->next() || rs->getString(1) != "1"){
             ///必须管理结果集，否则内存泄漏
            statement->closeResultSet(rs);
            return 1;
        }
        ///必须管理结果集，否则内存泄漏
        statement->closeResultSet(rs);


    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return 0;
}


int DAORiskCtrl::CheckAmount(const std::string &strMchntNo, const std::string &strCardNo, const int nAmount, const std::string &strWtype)
{
    int rc = 1;
    try{
		Statement *statement = m_db_cmd->getStatement();

        ///inMerNo in varchar2,inCardNo in varchar2,Amount in number,wtype in varchar2,ErrorNo out int,ErrorSql out number
        statement->setSQL("BEGIN risk_user.pkg_accp_pay_risk.p_mer_withdraw_inspection(:1,:2,:3,:4,:5,:6); end;");

        statement->setString(1, strMchntNo);
        statement->setString(2, strCardNo);
        statement->setInt(3, nAmount);
        statement->setString(4, strWtype);
        statement->registerOutParam(5, OCCIINT, 4);
        statement->registerOutParam(6, OCCINUMBER, 10);

        int count = statement->execute();
        if (count > 0){
            rc = statement->getInt(5);
        }

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return rc;
}

//获取卡白名单限额信息
int DAORiskCtrl::GetWhiteListLimit(const std::string &merch_no, const std::string &account_no, unsigned long &limit_amount)
{
    try{
        char sql[1024] = {0};
        sprintf(sql, "select value from risk_user.t_white_list \
                 where merno= '%s' and cardno = '%s' and type_id = \
                 (select type_id from risk_user.t_white_list_type \
                 where usertype=3 and wtype = '0' and valueType = '10' \
                 and acctype = '2' and cardtype = '1')", merch_no.c_str(), account_no.c_str());

        ResultSet *rs = m_db_cmd->executeQuery(sql);
		if (!rs->next())
		{
		    PLOG_INFO("GetWhiteListLimit error: merch_no=%s, account_no:%s", merch_no.c_str(), account_no.c_str());
			return 1;
		}

        limit_amount = rs->getInt(1);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return 0;
}

//获取商户风险等级
int DAORiskCtrl::GetMerchRiskLevel(const std::string &merch_no, std::string &risk_level)
{
    try{
        char sql[1024] = {0};
        sprintf(sql, "select risk_level from crm_user.t_customers \
                 where customer_id = \
                 (select distinct customer_id from nac_user.t_mer_busi \
                 where merchno='%s' ) ", merch_no.c_str());

        ResultSet *rs = m_db_cmd->executeQuery(sql);
		if (!rs->next())
		{
            PLOG_INFO("GetMerchRiskLevel error: merch_no=%s", merch_no.c_str());
		    risk_level = "F";
			return 1;
		}

        risk_level = rs->getString(1);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return 0;
}

//获取风险等级提现限额
int DAORiskCtrl::GetRiskLevelLimit(const std::string &risk_level, unsigned long &limit_amount)
{
    try{
        int risk_rank = 0;
        switch (risk_level[0]){
        case 'A':
            risk_rank = 21;
            break;
        case 'B':
            risk_rank = 22;
            break;
        case 'C':
            risk_rank = 23;
            break;
        case 'D':
            risk_rank = 24;
            break;
        case 'E':
            risk_rank = 25;
            break;
        default:
            risk_rank = 26;
            break;
        }

        char sql[1024] = {0};
        sprintf(sql, "Select nvl(value, -1) from risk_user.t_risk_param \
                 Where risk_rank = '%d'  and valuetype = '10'\
                 and acctype = '2' and cardtype = '1' ", risk_rank);

        ResultSet *rs = m_db_cmd->executeQuery(sql);
		if (!rs->next())
		{
		    limit_amount = 9999999999;
			return 0;
		}

        limit_amount = rs->getInt(1);
        if (limit_amount == -1){
            limit_amount = 9999999999;
        }

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return 0;
}


//获取卡当天交易总金额
int DAORiskCtrl::GetCardDayTotalAmout(const std::string &merch_no, const std::string &account_no, unsigned long &total_amount)
{
    try{
        char sql[1024] = {0};
        sprintf(sql, "select nvl(sum(dtl.trx_amt),0) from acc_user.ev_pay_dtl dtl \
                 where dtl.pay_dt = trunc(sysdate) and dtl.Rcv_Acct_No = '%s' \
                 and (dtl.status='0' or dtl.status='1') ",  account_no.c_str());

        ResultSet *rs = m_db_cmd->executeQuery(sql);
		if (!rs->next())
		{
		    total_amount = 0;
		    PLOG_INFO("GetCardDayTotalAmout error: account_no=%s", account_no.c_str());
			return 1;
		}

        total_amount = rs->getInt(1);

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return 0;
}
