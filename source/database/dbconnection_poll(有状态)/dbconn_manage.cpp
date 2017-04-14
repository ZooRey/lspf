#include <stdio.h>
#include <iostream>
using namespace std;

#include <occi.h>
using namespace oracle::occi;

#include "database/dbconn_manage.h"
#include "database/dbexception.h"

DBConnManage *DBConnManage::m_manage = NULL;

void DBConnManage::init()
{
	m_env = Environment::createEnvironment(
			Environment::Mode(Environment::OBJECT|Environment::THREADED_MUTEXED));

	if (NULL == m_env) {
		throw DBException(DBException::ENVIRONMENT_ERROR);
	}
}

bool DBConnManage::creatConnectionPool(const string  &dbname, const string &user, const string &passwd,
                                        const int &minConn, const int &maxConn, const int &inrConn)
{
    ///创建连接池
	ConnectionPool *connectionPool_ = m_env->createConnectionPool(user, passwd, dbname, minConn, maxConn, inrConn);
	if (NULL == connectionPool_) {
		return false;
	}

    ///设置连接池超时时间
	connectionPool_->setTimeOut(3600);

    DBConnPool *pDBConnPool = new DBConnPool(dbname, user, passwd, connectionPool_);

	m_vecPool.push_back(pDBConnPool);

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

void DBConnManage::fini()
{
    for (unsigned int i=0; i<m_vecPool.size(); i++)
    {
        m_env->terminateConnectionPool(m_vecPool[i]->getConnectionPool());
    }

    Environment::terminateEnvironment(m_env);
}
