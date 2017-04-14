#include <stdio.h>
#include <iostream>
using namespace std;

#include <occi.h>
using namespace oracle::occi;

#include "database/dbconn_manage.h"
#include "database/dbexception.h"
#include "dbconn_check.h"

DBConnManage *DBConnManage::m_manage = NULL;

void DBConnManage::init()
{
    try{
        m_env = Environment::createEnvironment(
                Environment::Mode(Environment::OBJECT|Environment::THREADED_MUTEXED));

        if (NULL == m_env) {
            throw DBException(DBException::ENVIRONMENT_ERROR);
        }
    }catch(SQLException &sqlExcp){
		throw DBException(DBException::ENVIRONMENT_ERROR);
	}
}

bool DBConnManage::creatConnectionPool(const string  &dbname, const string &user, const string &passwd,
                                        const int &minConn, const int &maxConn, const int &inrConn)
{
    try{
        ///创建无状态连接池
        StatelessConnectionPool *connection_pool= m_env->createStatelessConnectionPool(
                                user, passwd, dbname, maxConn, minConn, inrConn, StatelessConnectionPool::HOMOGENEOUS);

        ///设置连接池超时时间
        connection_pool->setTimeOut( 600 );
        StatelessConnectionPool::BusyOption option = StatelessConnectionPool::FORCEGET;
        connection_pool->setBusyOption(option);

        DBConnPool *pDBConnPool = new DBConnPool(dbname, user, passwd, connection_pool);

        m_vecPool.push_back(pDBConnPool);

    }catch(SQLException &sqlExcp){
		return false;
	}

    return true;
}

DBConnPool *DBConnManage::getDBConnPool(const unsigned int index)
{
    if (index >= m_vecPool.size())
    {
        return NULL;
    }

    return m_vecPool[index];
}

/// @brief 启动检测连接线程
void DBConnManage::CheckConnection(const int timeout)
{
    DBConnCheckThread::Init(m_vecPool.size());

    DBConnCheckThread::Instance()->Start();
}

void DBConnManage::fini()
{
    try{
        for (unsigned int i=0; i<m_vecPool.size(); i++)
        {
            m_env->terminateStatelessConnectionPool(m_vecPool[i]->getConnectionPool());
            delete m_vecPool[i];
        }

        Environment::terminateEnvironment(m_env);

    }catch(SQLException &sqlExcp){
		//PLOG_ERROR("SQLException %d:%s. sql=[%s]\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
        //return -1;
	}
}
