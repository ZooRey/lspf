#include "common/time_utility.h"
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <occi/occi.h>
#include "database/db_oracle.h"

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

void TestDB(){

    TimerClock clock;

    try{
        std::cout << "Elapsed11--" << clock.Elapsed() << std::endl;
        DBCommand db_cmd;
        std::cout << "Elapsed22--" << clock.Elapsed() << std::endl;

        ResultSet *rs = db_cmd.executeQuery("select sysdate from dual");
        if (rs->next())
        {
            std::cout << "sysdate = " << rs->getString(1) << std::endl;
        }

        std::cout << "Elapsed33--" << clock.Elapsed() << std::endl;
    }
    catch(SQLException &sqlExcp)
    {
        printf("SQLException %d:%s.\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        exit(-1);
    }

	std::cout << "Elapsed44" << clock.Elapsed() << std::endl;
}


int TestDB2(){
    std::string szUserName("acc_user");        // 用户名
    std::string szPassword("123456");       // 密码
    std::string szConnectString("172.20.11.100:1521/mpos");  // 连接字符串

    Environment *pEnv = NULL;   // OCCI上下文环境
    Connection *pConn = NULL;   // 数据库连接
    Statement *pStmt = NULL;    // Statement对象
    std::string strTemp;

    try {
        TimerClock clock;
        std::cout << "Elapsed1---" << clock.Elapsed() << std::endl;

        // 创建OCCI上下文环境
        pEnv = Environment::createEnvironment(
            Environment::Mode(Environment::OBJECT|Environment::THREADED_MUTEXED));
        if (NULL == pEnv) {
            printf("createEnvironment error.\n");
            return -1;
        }
        std::cout << "Elapsed2---" << clock.Elapsed() << std::endl;
        // 创建数据库连接
        pConn = pEnv->createConnection(szUserName, szPassword, szConnectString);
        if(NULL == pConn) {
            printf("createConnection error.\n");
            return -1;
        }
        std::cout << "Elapsed3---" << clock.Elapsed() << std::endl;
        // 创建Statement对象
        pStmt = pConn->createStatement();
        if(NULL == pStmt) {
            printf("createStatement error.\n");
            return -1;
        }
        std::cout << "Elapsed4---" << clock.Elapsed() << std::endl;
        //--------查询---------
        // 查询数据库时间
        ResultSet *pRs = pStmt->executeQuery(
            "SELECT TO_CHAR(SYSDATE, 'YYYY-MM-DD HH24:MI:SS') FROM DUAL");
        if(pRs->next()) {
            strTemp = pRs->getString(1);
            printf("db time:%s.\n", strTemp.c_str());
        }
        std::cout << "Elapsed5---" << clock.Elapsed() << std::endl;
        //pStmt->closeResultSet(pRs);
        //----------------------
        // 终止Statement对象
        pConn->terminateStatement(pStmt);
        // 断开数据库连接
        pEnv->terminateConnection(pConn);
        // 释放OCCI上下文环境
        Environment::terminateEnvironment(pEnv);
    }
    // 捕获数据库操作异常
    catch(SQLException &sqlExcp) {
        printf("SQLException %d:%s.\n",
            sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
    }
    // 捕获其他异常
    catch(exception &ex) {
        printf("other exception %s.\n", ex.what());
        return -1;
    }
    return 0;
}

int TestStatelessPool(){
    std::string szUserName("acc_user");        // 用户名
    std::string szPassword("123456");       // 密码
    std::string szConnectString("172.20.11.100:1521/mpos");  // 连接字符串

    Environment *pEnv = NULL;   // OCCI上下文环境
    Connection *pConn = NULL;   // 数据库连接
    Statement *pStmt = NULL;    // Statement对象
    std::string strTemp;

    try {
        TimerClock clock;
        std::cout << "Elapsed1---" << clock.Elapsed() << std::endl;

        // 创建OCCI上下文环境
        pEnv = Environment::createEnvironment(
            Environment::Mode(Environment::OBJECT|Environment::THREADED_MUTEXED));
        if (NULL == pEnv) {
            printf("createEnvironment error.\n");
            return -1;
        }
        std::cout << "Elapsed2---" << clock.Elapsed() << std::endl;

        ///创建连接池
        ///创建无状态连接池
        StatelessConnectionPool *connectionPool_= pEnv->createStatelessConnectionPool(
                                szUserName, szPassword, szConnectString, 2, 1, 1, StatelessConnectionPool::HOMOGENEOUS);

        ///设置连接池超时时间
        connectionPool_->setTimeOut( 60 );
        StatelessConnectionPool::BusyOption option = StatelessConnectionPool::FORCEGET;
        connectionPool_->setBusyOption(option);

        // 创建数据库连接
        pConn = connectionPool_->getConnection();
        if(NULL == pConn) {
            printf("createConnection error.\n");
            return -1;
        }
        std::cout << "Elapsed3---" << clock.Elapsed() << std::endl;
        // 创建Statement对象
        pStmt = pConn->createStatement();
        if(NULL == pStmt) {
            printf("createStatement error.\n");
            return -1;
        }
        std::cout << "Elapsed4---" << clock.Elapsed() << std::endl;
        //--------查询---------
        // 查询数据库时间
        ResultSet *pRs = pStmt->executeQuery(
            "SELECT TO_CHAR(SYSDATE, 'YYYY-MM-DD HH24:MI:SS') FROM DUAL");
        if(pRs->next()) {
            strTemp = pRs->getString(1);
            printf("db time:%s.\n", strTemp.c_str());
        }
        std::cout << "Elapsed5---" << clock.Elapsed() << std::endl;
        //pStmt->closeResultSet(pRs);
        //----------------------
        // 终止Statement对象
        pConn->terminateStatement(pStmt);
        // 断开数据库连接
        //connectionPool_->terminateConnection(pConn);
        connectionPool_->releaseConnection(pConn);

        pEnv->terminateStatelessConnectionPool(connectionPool_);
        // 释放OCCI上下文环境
        Environment::terminateEnvironment(pEnv);
    }
    // 捕获数据库操作异常
    catch(SQLException &sqlExcp) {
        printf("SQLException %d:%s.\n",
            sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
    }
    // 捕获其他异常
    catch(exception &ex) {
        printf("other exception %s.\n", ex.what());
        return -1;
    }
    return 0;
}


int TestPool(){
    std::string szUserName("acc_user");        // 用户名
    std::string szPassword("123456");       // 密码
    std::string szConnectString("10.0.51.35:1521/mpos");  // 连接字符串

    Environment *pEnv = NULL;   // OCCI上下文环境
    Connection *pConn = NULL;   // 数据库连接
    Statement *pStmt = NULL;    // Statement对象
    std::string strTemp;

    try {
        TimerClock clock;
        std::cout << "Elapsed1---" << clock.Elapsed() << std::endl;

        // 创建OCCI上下文环境
        pEnv = Environment::createEnvironment(
            Environment::Mode(Environment::OBJECT|Environment::THREADED_MUTEXED));
        if (NULL == pEnv) {
            printf("createEnvironment error.\n");
            return -1;
        }
        std::cout << "Elapsed2---" << clock.Elapsed() << std::endl;

        ///创建连接池
        ConnectionPool *connectionPool_ = pEnv->createConnectionPool(szUserName, szPassword, szConnectString, 1, 5, 1);
        std::cout << "Elapsed002---" << clock.Elapsed() << std::endl;

        // 创建数据库连接
        pConn = connectionPool_->createConnection(szUserName, szPassword);
        if(NULL == pConn) {
            printf("createConnection error.\n");
            return -1;
        }
        std::cout << "Elapsed3---" << clock.Elapsed() << std::endl;
        // 创建Statement对象
        pStmt = pConn->createStatement();
        if(NULL == pStmt) {
            printf("createStatement error.\n");
            return -1;
        }
        std::cout << "Elapsed4---" << clock.Elapsed() << std::endl;
        //--------查询---------
        // 查询数据库时间
        ResultSet *pRs = pStmt->executeQuery(
            "SELECT TO_CHAR(SYSDATE, 'YYYY-MM-DD HH24:MI:SS') FROM DUAL");
        if(pRs->next()) {
            strTemp = pRs->getString(1);
            printf("db time:%s.\n", strTemp.c_str());
        }
        std::cout << "Elapsed5---" << clock.Elapsed() << std::endl;
        pStmt->closeResultSet(pRs);
        //----------------------
        // 终止Statement对象
        pConn->terminateStatement(pStmt);
        // 断开数据库连接
        pEnv->terminateConnection(pConn);
        // 释放OCCI上下文环境
        Environment::terminateEnvironment(pEnv);
    }
    // 捕获数据库操作异常
    catch(SQLException &sqlExcp) {
        printf("SQLException %d:%s.\n",
            sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
    }
    // 捕获其他异常
    catch(exception &ex) {
        printf("other exception %s.\n", ex.what());
        return -1;
    }
    return 0;
}

//获取手续费计算参数
int GetSettleFeeParam()
{
    try
    {
        DBCommand db_cmd;
        double fee_rate; int min_fee; int max_fee;
        char sql[2048] = {0};
        sprintf(sql, "select t.sett_fee, t.top_amount, t.below_amount from crm_user.t_sett_fee t where "
                "t.Mer_Fee_Id = '2141698' and t.customer_level = '2'");

        ResultSet *rs = db_cmd.executeQuery(sql);
        if(!rs->next())
        {
            sprintf(sql, "select t.sett_fee, t.top_amount, t.below_amount from crm_user.t_sett_fee t where "
                    " t.Mer_Fee_Id = '2141698' order by t.customer_level desc");

            std::cout << "Mer_Fee_Id111" << std::endl;
            rs = db_cmd.executeQuery(sql);
            if(!rs->next())
            {
                return -1;
            }

            std::cout << "Mer_Fee_Id222222" << std::endl;
        }

        std::cout << "GetSettleFeeParam~~~~~~~" << std::endl;
        fee_rate = atof(rs->getString(1).c_str());
        std::cout << "fee_rate=" << fee_rate << std::endl;
        min_fee = atoi(rs->getString(2).c_str());
        std::cout << "min_fee=" <<min_fee << std::endl;
        max_fee = atoi(rs->getString(3).c_str());
        std::cout << "max_fee=" <<max_fee << std::endl;
    }
    catch(SQLException &sqlExcp)
    {
        std::cout << "SQLException" << std::endl;
        return -1;
    }
    catch(DBException &dbExcp)
    {
        std::cout << "DBException" << std::endl;
        return -1;
    }

    return 0;
}

int main() {
    //TestDB2();
   // TestStatelessPool();
    //TestPool();

	if (!createPool("172.20.11.100:1521/mpos", "acc_user", "123456", 1, 20)){
		cout << "createPool error " << endl;
	}

	TestDB();

	return 0;

}
