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
    std::string szUserName("acc_user");        // �û���
    std::string szPassword("123456");       // ����
    std::string szConnectString("172.20.11.100:1521/mpos");  // �����ַ���

    Environment *pEnv = NULL;   // OCCI�����Ļ���
    Connection *pConn = NULL;   // ���ݿ�����
    Statement *pStmt = NULL;    // Statement����
    std::string strTemp;

    try {
        TimerClock clock;
        std::cout << "Elapsed1---" << clock.Elapsed() << std::endl;

        // ����OCCI�����Ļ���
        pEnv = Environment::createEnvironment(
            Environment::Mode(Environment::OBJECT|Environment::THREADED_MUTEXED));
        if (NULL == pEnv) {
            printf("createEnvironment error.\n");
            return -1;
        }
        std::cout << "Elapsed2---" << clock.Elapsed() << std::endl;
        // �������ݿ�����
        pConn = pEnv->createConnection(szUserName, szPassword, szConnectString);
        if(NULL == pConn) {
            printf("createConnection error.\n");
            return -1;
        }
        std::cout << "Elapsed3---" << clock.Elapsed() << std::endl;
        // ����Statement����
        pStmt = pConn->createStatement();
        if(NULL == pStmt) {
            printf("createStatement error.\n");
            return -1;
        }
        std::cout << "Elapsed4---" << clock.Elapsed() << std::endl;
        //--------��ѯ---------
        // ��ѯ���ݿ�ʱ��
        ResultSet *pRs = pStmt->executeQuery(
            "SELECT TO_CHAR(SYSDATE, 'YYYY-MM-DD HH24:MI:SS') FROM DUAL");
        if(pRs->next()) {
            strTemp = pRs->getString(1);
            printf("db time:%s.\n", strTemp.c_str());
        }
        std::cout << "Elapsed5---" << clock.Elapsed() << std::endl;
        //pStmt->closeResultSet(pRs);
        //----------------------
        // ��ֹStatement����
        pConn->terminateStatement(pStmt);
        // �Ͽ����ݿ�����
        pEnv->terminateConnection(pConn);
        // �ͷ�OCCI�����Ļ���
        Environment::terminateEnvironment(pEnv);
    }
    // �������ݿ�����쳣
    catch(SQLException &sqlExcp) {
        printf("SQLException %d:%s.\n",
            sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
    }
    // ���������쳣
    catch(exception &ex) {
        printf("other exception %s.\n", ex.what());
        return -1;
    }
    return 0;
}

int TestStatelessPool(){
    std::string szUserName("acc_user");        // �û���
    std::string szPassword("123456");       // ����
    std::string szConnectString("172.20.11.100:1521/mpos");  // �����ַ���

    Environment *pEnv = NULL;   // OCCI�����Ļ���
    Connection *pConn = NULL;   // ���ݿ�����
    Statement *pStmt = NULL;    // Statement����
    std::string strTemp;

    try {
        TimerClock clock;
        std::cout << "Elapsed1---" << clock.Elapsed() << std::endl;

        // ����OCCI�����Ļ���
        pEnv = Environment::createEnvironment(
            Environment::Mode(Environment::OBJECT|Environment::THREADED_MUTEXED));
        if (NULL == pEnv) {
            printf("createEnvironment error.\n");
            return -1;
        }
        std::cout << "Elapsed2---" << clock.Elapsed() << std::endl;

        ///�������ӳ�
        ///������״̬���ӳ�
        StatelessConnectionPool *connectionPool_= pEnv->createStatelessConnectionPool(
                                szUserName, szPassword, szConnectString, 2, 1, 1, StatelessConnectionPool::HOMOGENEOUS);

        ///�������ӳس�ʱʱ��
        connectionPool_->setTimeOut( 60 );
        StatelessConnectionPool::BusyOption option = StatelessConnectionPool::FORCEGET;
        connectionPool_->setBusyOption(option);

        // �������ݿ�����
        pConn = connectionPool_->getConnection();
        if(NULL == pConn) {
            printf("createConnection error.\n");
            return -1;
        }
        std::cout << "Elapsed3---" << clock.Elapsed() << std::endl;
        // ����Statement����
        pStmt = pConn->createStatement();
        if(NULL == pStmt) {
            printf("createStatement error.\n");
            return -1;
        }
        std::cout << "Elapsed4---" << clock.Elapsed() << std::endl;
        //--------��ѯ---------
        // ��ѯ���ݿ�ʱ��
        ResultSet *pRs = pStmt->executeQuery(
            "SELECT TO_CHAR(SYSDATE, 'YYYY-MM-DD HH24:MI:SS') FROM DUAL");
        if(pRs->next()) {
            strTemp = pRs->getString(1);
            printf("db time:%s.\n", strTemp.c_str());
        }
        std::cout << "Elapsed5---" << clock.Elapsed() << std::endl;
        //pStmt->closeResultSet(pRs);
        //----------------------
        // ��ֹStatement����
        pConn->terminateStatement(pStmt);
        // �Ͽ����ݿ�����
        //connectionPool_->terminateConnection(pConn);
        connectionPool_->releaseConnection(pConn);

        pEnv->terminateStatelessConnectionPool(connectionPool_);
        // �ͷ�OCCI�����Ļ���
        Environment::terminateEnvironment(pEnv);
    }
    // �������ݿ�����쳣
    catch(SQLException &sqlExcp) {
        printf("SQLException %d:%s.\n",
            sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
    }
    // ���������쳣
    catch(exception &ex) {
        printf("other exception %s.\n", ex.what());
        return -1;
    }
    return 0;
}


int TestPool(){
    std::string szUserName("acc_user");        // �û���
    std::string szPassword("123456");       // ����
    std::string szConnectString("10.0.51.35:1521/mpos");  // �����ַ���

    Environment *pEnv = NULL;   // OCCI�����Ļ���
    Connection *pConn = NULL;   // ���ݿ�����
    Statement *pStmt = NULL;    // Statement����
    std::string strTemp;

    try {
        TimerClock clock;
        std::cout << "Elapsed1---" << clock.Elapsed() << std::endl;

        // ����OCCI�����Ļ���
        pEnv = Environment::createEnvironment(
            Environment::Mode(Environment::OBJECT|Environment::THREADED_MUTEXED));
        if (NULL == pEnv) {
            printf("createEnvironment error.\n");
            return -1;
        }
        std::cout << "Elapsed2---" << clock.Elapsed() << std::endl;

        ///�������ӳ�
        ConnectionPool *connectionPool_ = pEnv->createConnectionPool(szUserName, szPassword, szConnectString, 1, 5, 1);
        std::cout << "Elapsed002---" << clock.Elapsed() << std::endl;

        // �������ݿ�����
        pConn = connectionPool_->createConnection(szUserName, szPassword);
        if(NULL == pConn) {
            printf("createConnection error.\n");
            return -1;
        }
        std::cout << "Elapsed3---" << clock.Elapsed() << std::endl;
        // ����Statement����
        pStmt = pConn->createStatement();
        if(NULL == pStmt) {
            printf("createStatement error.\n");
            return -1;
        }
        std::cout << "Elapsed4---" << clock.Elapsed() << std::endl;
        //--------��ѯ---------
        // ��ѯ���ݿ�ʱ��
        ResultSet *pRs = pStmt->executeQuery(
            "SELECT TO_CHAR(SYSDATE, 'YYYY-MM-DD HH24:MI:SS') FROM DUAL");
        if(pRs->next()) {
            strTemp = pRs->getString(1);
            printf("db time:%s.\n", strTemp.c_str());
        }
        std::cout << "Elapsed5---" << clock.Elapsed() << std::endl;
        pStmt->closeResultSet(pRs);
        //----------------------
        // ��ֹStatement����
        pConn->terminateStatement(pStmt);
        // �Ͽ����ݿ�����
        pEnv->terminateConnection(pConn);
        // �ͷ�OCCI�����Ļ���
        Environment::terminateEnvironment(pEnv);
    }
    // �������ݿ�����쳣
    catch(SQLException &sqlExcp) {
        printf("SQLException %d:%s.\n",
            sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        return -1;
    }
    // ���������쳣
    catch(exception &ex) {
        printf("other exception %s.\n", ex.what());
        return -1;
    }
    return 0;
}

//��ȡ�����Ѽ������
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
