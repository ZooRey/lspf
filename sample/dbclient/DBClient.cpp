#include <iostream>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <cstdio>
#include <unistd.h>
#include <occi.h>
#include <boost/progress.hpp>
#include "database/dbcommand.h"
#include "database/dbexception.h"

#include "log/log.h"

using namespace std;
using namespace oracle::occi;

DBConnManage *pDBConnManage = DBConnManage::instance();

bool createPool(const string &strDBName, const string &strUserName, const string &strPassword, int nMinConn, int nMaxConn){

	pDBConnManage->init();

    if (!pDBConnManage->creatConnectionPool(strDBName, strUserName, strPassword, nMinConn, nMaxConn))
    {
        cout << "creatConnectionPool ERROR: " << endl;
        exit(-1);
    }

	return true;
}

//获取代付请求付款状态
int getPaymentStatus()
{
    int iRet = 5;
    std::string strRequestId = "170308194923282828";
	std::string sql;
    try{
        DBCommand db_cmd;
        sql = std::string("select a.TRX_AMT as payamt, to_char(a.Last_Updt_Dt_Tm, 'YYYY-MM-DD HH24:MI:SS') as datetime, \
                       status,ret_msg from EV_PAY_ACC a  where a.PAY_RQS_ID = '") + strRequestId + "'";

		Statement *statement = db_cmd.getStatement();
		statement->setSQL(sql);
        ResultSet *rs = statement->executeQuery();

		if (!rs->next()){
            statement->closeResultSet(rs);
			PLOG_ERROR("getPaymentStatus:%s\n", sql.c_str());
			return 1;
		}

        int nTotalAmt = rs->getInt(1);
        std::string strTime = rs->getString(2);
        std::string strStatus = rs->getString(3);
        std::string ret_msg = rs->getString(4);

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

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str(), sql.c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

    return iRet;
}

int TestQuery()
{
    try
    {
        DBCommand db_cmd;

        Statement *statement = db_cmd.getStatement();

        int seqno = 0;

        printf("TestQuery start...\n");

        string strSql;
        {
            boost::progress_timer t;


            strSql = string("select withdraw_id from t_acc_withdraw_log where withdraw_id = 9999999999");

            ResultSet *pRs = statement->executeQuery(strSql);

            if (!pRs->next())
            {
                printf("no record\n");
                statement->closeResultSet(pRs);
                return 1;
            }

            seqno = pRs->getInt(1);

            ///必须管理结果集，否则内存泄漏
            statement->closeResultSet(pRs);
        }

        printf("TestQuery end...\n");

    }
    catch(SQLException &sqlExcp)
    {
        PLOG_ERROR("TestQuery SQLException %d:%s.\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return (-1);
    }catch(DBException &x){
		PLOG_ERROR("DBException :%s.\n", x.what());
        return -1;
	}


    return 0;
}

int TestQuery2()
{
    try
    {
        DBCommand db_cmd;

        int seqno = 0;

        printf("TestQuery2 start...\n");

        string strSql;
        {
            boost::progress_timer t;


            strSql = string("select withdraw_id from t_acc_withdraw_log where withdraw_id = 9999999999");

            ///可以不显式关闭结果集，析构时将自动关闭
            ResultSet *pRs = db_cmd.executeQuery(strSql);

            if (!pRs->next())
            {
                printf("no record\n");
                return 1;
            }

            seqno = pRs->getInt(1);

            ///可以不显式关闭结果集，析构时将自动关闭
            //db_cmd.closeResultSet(pRs);
        }


    }
    catch(SQLException &sqlExcp)
    {
        PLOG_ERROR("TestQuery SQLException %d:%s.\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return (-1);
    }catch(DBException &x){
		PLOG_ERROR("DBException :%s.\n", x.what());
        return -1;
	}

    printf("TestQuery2 end...\n");
    return 0;
}

int TestUpdate()
{
    try
    {
        printf("TestUpdate start...");
        DBCommand MDBCmd_;
        int seqno = 0;
        string strSql;
        {
            boost::progress_timer t;

            char sql[2048] = {0};
            sprintf(sql, "insert into T_ACC_TXN_LOG_INF(TXN_LOG_NO, TXN_CHL, TXN_COD, "
                    "TXN_SU_COD, txn_acc_dat, TXN_LTL_MERCHNO, TXN_LTL_TERMNO, TXN_LTL_AMOUNT, "
                    "TXN_LTL_BATCHNO, TXN_LTL_VOUCHNO, TXN_LTL_AUTHCODE, TXN_LTL_REFNO, TXN_PAY_CHL, "
                    "TXN_LTL_AUTOID, TXN_DAT, ORDER_ID) values('%d', '10','2001','104', "
                    "to_date('20161014154713', 'yyyymmddhh24miss'),'001100071574018','20966905',"
                    "'000000015800','000001','000015','154718','023581769923','100', '321684', sysdate, to_number('1476431218467553'))", seqno);

            printf("sql=%s\n", sql);
            strSql.assign(sql);

            if (MDBCmd_.executeUpdate(strSql) != 0){
                printf("executeUpdate ERROR\n");
                return 1;
            }
        }

        sleep(1);
    }
    catch(SQLException &sqlExcp)
    {
        printf("TestUpdate SQLException %d:%s.\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return (-1);
    }

    printf("TestUpdate end...");

    return 0;
}

void thread_run(int num){

    int i = 0;
	for(i=0; i<1000000; i++)
	{
	    getPaymentStatus();
		//TestQuery();
		printf("i=%d\n", i);
		//TestQuery2();
	}
}

int TestProc(){
    try{
        DBCommand db_cmd;

        Statement *statement = db_cmd.getStatement();

        Number number(1000);

        ///inMerNo in varchar2,inCardNo in varchar2,Amount in number,wtype in varchar2,ErrorNo out int,ErrorSql out number
        statement->setSQL("BEGIN risk_user.pkg_accp_pay_risk.p_mer_withdraw_inspection(:1,:2,:3,:4,:5,:6); end;");

        statement->setString(1, "001440332334700");
        statement->setString(2, "6228480402564890021");
        statement->setNumber(3, number);
        statement->setString(4, "1");
        statement->registerOutParam(5, OCCIINT, 4);
        statement->registerOutParam(6, OCCINUMBER, 10);
        //statement->registerOutParam(5, TYPE::OCCISTRING, );

        int count = statement->execute();
        cout << "count= " << count << endl;
        cout << "code = " << statement->getInt(5) << endl;
        //cout << "code = " << statement->getInt(5) << "\nerror= " << statement->getNumber(6) << endl;
    }
    catch(SQLException &sqlExcp)
    {
        printf("SQLException %d:%s\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
    }
    catch(DBException &dbExcp)
    {
        printf("DBException :%s.\n", dbExcp.what());
        return -1;
    }
    return 0;
}

int TestRiskProc(){
    try
    {
        DBCommand db_cmd;

        Statement *statement = db_cmd.getStatement();

        statement->setSQL("BEGIN risk_user.P_FREEZE_FLAG(:1, :2, :3, :4); end;");

        statement->setString(1, "11111111");
        statement->setString(2, "22222222222222");
        statement->setInt(3, 111);
        statement->registerOutParam(4, OCCISTRING, 1, "");

        statement->execute();

        std::string status = statement->getString(4);

        printf("status=%s\n", status.c_str());

    }
    catch(SQLException &sqlExcp)
    {
        printf("SQLException %d:%s\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
    }
    catch(DBException &dbExcp)
    {
        printf("DBException :%s.\n", dbExcp.what());
        return -1;
    }

    return 0;
}


int GetAccountInfo()
{
    try{
        DBCommand db_cmd;
        char sql[1024] = {0};
        sprintf(sql, "select b.bank_code,"
              " b.card_no,"
              " a.acctype,"
              " b.bank_acc_name,"
              " b.bank_sub_name,"
              " b.bank_lineno,"
              " a.account_name"
              " from crm_user.T_SETTINFOS a, crm_user.T_BANK_ACCOUNTS b "
              " where a.bank_acc_id = b.bank_acc_id and a.account_id in "
              " (select a.pay_ac_no from T_ACC_PROFILE a "
              " where a.mer_id = '001100048332611')");

        ResultSet *rs = db_cmd.executeQuery(sql);
        if (!rs->next()){
            PLOG_ERROR("GetAccountInfo  no data");
            return 1;
        }

        std::string m_acc_bankno  = rs->getString(1);
        std::string m_acc_no = rs->getString(2);      //银行账户ID
        std::string m_acc_type = rs->getString(3);      //账号类型
        std::string m_acc_name = rs->getString(4);  //开户名
        std::string m_acc_bankname = rs->getString(5);
        std::string m_acc_bank_line_no = rs->getString(6);

        if (m_acc_type == "1")
        {
            m_acc_name.clear();
            m_acc_name = rs->getString(7);
        }

        std::cout << "m_acc_name=" << m_acc_name<< std::endl;

    }catch(SQLException &sqlExcp){
		PLOG_ERROR("SQLException %d:%s\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
	}catch(DBException &dbExcp){
		PLOG_ERROR("DBException :%s.\n", dbExcp.what());
        return -1;
	}

	return 0;
}


int main() {
	if (!createPool("172.20.11.100:1521/mpos", "acc_user", "123456", 1, 10)){
		cout << "createPool error " << endl;
		return -1;
	}

    //TestQuery();
    //thread_run(1);
    //TestRiskProc();

	boost::progress_timer t;

    for(int i=0;i<200000;i++){
        boost::thread_group group;
         for(int num=0;num<10;num++)
            group.create_thread(boost::bind(&thread_run, num));
        group.join_all();
    }


    DBConnManage::instance()->fini();

    delete DBConnManage::instance();

    return 0;
}
