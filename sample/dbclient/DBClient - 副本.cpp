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

int TestQuery1()
{

    sleep(1);
    try
    {
        DBCommand MDBCmd_;
        int seqno;

        printf("TestQuery start...\n");

        string strSql;
        {
            boost::progress_timer t;


            strSql = string("select 1 from dual");

            ResultSet *pRs = MDBCmd_.executeQuery(strSql);

            if (!pRs->next())
            {
                printf("no record\n");
                return 1;
            }

            seqno = pRs->getInt(1);
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

    printf("TestQuery end...\n");
    return 0;
}
Environment * env = NULL;
StatelessConnectionPool *connectionPool_ = NULL;
int TestQuery()
{
    Connection *conn;

    try
    {
        {
            printf("createConnection start...\n");
            boost::progress_timer t1;

            printf("111111BusyConnections = %d\n", connectionPool_->getBusyConnections());
            printf("111111OpenConnections = %d\n", connectionPool_->getOpenConnections());
            conn=connectionPool_->getConnection();
            printf("222222BusyConnections = %d\n", connectionPool_->getBusyConnections());
            printf("222222OpenConnections = %d\n", connectionPool_->getOpenConnections());
            printf("createConnection end...\n");
        }

        int seqno;

        printf("TestQuery start...\n");

        string strSql;
        {
            boost::progress_timer t2;

            Statement * st = conn->createStatement();

            strSql = string("select 1 from dual");

            ResultSet *pRs = st->executeQuery(strSql);

            if (!pRs->next())
            {
                printf("no record\n");
                connectionPool_->terminateConnection(conn);
                return 1;
            }

            seqno = pRs->getInt(1);
        }

    }
    catch(SQLException &sqlExcp)
    {
        connectionPool_->terminateConnection(conn);
        PLOG_ERROR("TestQuery SQLException %d:%s.\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return (-1);
    }catch(DBException &x){
        connectionPool_->terminateConnection(conn);
		PLOG_ERROR("DBException :%s.\n", x.what());
        return -1;
	}

    conn->flushCache();

    connectionPool_->releaseConnection(conn);

    printf("TestQuery end...\n");
    return 0;
}

int TestUpdate()
{
    try
    {
        printf("TestUpdate start...");
        DBCommand MDBCmd_;
        int seqno;
        string strSql;
        {
            boost::progress_timer t;

            char sql[2048] = {0};
            sprintf(sql, "insert into T_ACC_TXN_LOG_INF(TXN_LOG_NO, TXN_CHL, TXN_COD, TXN_SU_COD, txn_acc_dat, TXN_LTL_MERCHNO, TXN_LTL_TERMNO, TXN_LTL_AMOUNT, TXN_LTL_BATCHNO, TXN_LTL_VOUCHNO, TXN_LTL_AUTHCODE, TXN_LTL_REFNO, TXN_PAY_CHL, TXN_LTL_AUTOID, TXN_DAT, ORDER_ID) values('%d', '10','2001','104',to_date('20161014154713', 'yyyymmddhh24miss'),'001100071574018','20966905','000000015800','000001','000015','154718','023581769923','100', '321684', sysdate, to_number('1476431218467553'))", seqno);

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

    int i;
	for(i=0; i<20; i++)
	{
	    printf("i=%d\n", i);
		TestQuery();
        sleep(i*10);
	}
}

void TestProc(){
	DBCommand db_cmd;

	Statement *statement = db_cmd.getStatement();

	///inMerNo in varchar2,inCardNo in varchar2,Amount in number,wtype in varchar2,ErrorNo out int,ErrorSql out number
	statement->setSQL("BEGIN risk_user.pkg_accp_pay_risk.p_mer_withdraw_inspection(:1,:2,:3,:4,:5,:6); end;");

	statement->setString(1, "001440332334700");
	statement->setString(2, "6228480402564890021");
	statement->setInt(3, 1000);
	statement->setString(4, "1");
	statement->registerOutParam(5, OCCIINT, 4);
	statement->registerOutParam(6, OCCINUMBER, 10);
	//statement->registerOutParam(5, TYPE::OCCISTRING, );

	int count = statement->execute();
	cout << "count= " << count << endl;
	cout << "code = " << statement->getInt(5) << endl;
	//cout << "code = " << statement->getInt(5) << "\nerror= " << statement->getNumber(6) << endl;
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

/*
	if (!createPool("10.0.51.35:1521/mpos", "acc_user", "123456", 10, 20)){
		cout << "createPool error " << endl;
	}
*/
    env = Environment::createEnvironment(//Environment::DEFAULT);
			Environment::Mode(Environment::OBJECT|Environment::THREADED_MUTEXED));

    ///创建连接池
	connectionPool_ = env->createStatelessConnectionPool("acc_user", "123456", "10.0.51.35:1521/mpos", 10, 1, 2,StatelessConnectionPool::HOMOGENEOUS);
	if (NULL == connectionPool_) {
		return 1;
	}

    ///设置连接池超时时间
	connectionPool_->setTimeOut(30);
    StatelessConnectionPool::BusyOption option = StatelessConnectionPool::FORCEGET;
    connectionPool_->setBusyOption(option);

	boost::progress_timer t;
	boost::thread_group group;

	for(int num=0;num<10;num++)
		group.create_thread(boost::bind(&thread_run, num));
	group.join_all();

}
