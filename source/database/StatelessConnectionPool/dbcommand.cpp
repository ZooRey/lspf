#include <string>
#include <list>
#include <map>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <cstdio>
using namespace std;

#include <stdint.h>

#include "database/dbconn_manage.h"
#include "database/dbexception.h"
#include "database/dbcommand.h"

#include "log/log.h"
#include "common/time_utility.h"

DBCommand::DBCommand(const int index)
                    : m_index(index),
                      bAutoCommit(true),
                      m_statement(NULL),
                      m_connection(NULL),
                      m_connPool(NULL)
{

}

DBCommand::~DBCommand()
{
    try{
        if (m_statement != NULL){
            // 终止Statement对象
            m_connection->terminateStatement(m_statement);
            m_statement == NULL;
        }

        if (m_connection != NULL){
            // 4、释放连接到连接池
            m_connection->flushCache();

            PLOG_DEBUG("releaseConnection...");

            //断开连接
            m_connPool->releaseConnection(m_connection);

            m_connection = NULL;

        }
	}catch(SQLException &sqlExcp)
	{
		PLOG_ERROR("SQLException %d:%s.\n", sqlExcp.getErrorCode(), sqlExcp.getMessage().c_str());
		m_connPool->terminateConnection(m_connection);
	}
}

void DBCommand::init()
{
    DBConnPool *pDBConnPool;
    DBConnManage *manage = DBConnManage::instance();

    PLOG_DEBUG("DBCommand::init begin...");

    pDBConnPool = manage->getDBConnPool(m_index);
    if (pDBConnPool == NULL)
    {
        throw DBException(DBException::INVALID_CONNECTION_POOL);
    }
    PLOG_DEBUG("DBCommand::getDBConnPool finish...");

    m_connPool = pDBConnPool->getConnectionPool();
    if (m_connPool == NULL)
    {
        throw DBException(DBException::INVALID_CONNECTION_POOL);
    }

    PLOG_DEBUG("DBCommand::getConnectionPool finish...");

    PLOG_DEBUG("BusyConnections = %d", m_connPool->getBusyConnections());
    PLOG_DEBUG("OpenConnections = %d", m_connPool->getOpenConnections());
    PLOG_DEBUG("MaxConnections = %d", m_connPool->getMaxConnections());

    m_connection = pDBConnPool->getConnection();
    if (m_connection == NULL)
    {
        throw DBException(DBException::INVALID_CONNECTION_POOL);
    }

    PLOG_DEBUG("DBCommand::getConnection finish...");
    /*
    PLOG_DEBUG("BusyConnections = %d", m_connPool->getBusyConnections());
    PLOG_DEBUG("MaxConnections = %d", m_connPool->getMaxConnections());
    PLOG_DEBUG("OpenConnections = %d", m_connPool->getOpenConnections());
    */

    m_statement = m_connection->createStatement();
    if (m_statement == NULL)
    {
        throw DBException(DBException::INVALID_STATEMENT);
    }

    PLOG_DEBUG("DBCommand::createStatement finish...");

    PLOG_DEBUG("DBCommand::init end...");
}

void DBCommand::terminate()
{
	if (m_statement != NULL)
	{
	    // 终止Statement对象
		m_connection->terminateStatement(m_statement);
		m_statement = NULL;
	}

    if (NULL != m_connection)
    {
		m_connPool->terminateConnection(m_connection);
		m_connection = NULL;
	}
}

Statement *DBCommand::getStatement()
{
    if (NULL == m_statement)
    {
		init();
	}

    m_statement->setAutoCommit(true);

	return m_statement;
}

int DBCommand::executeUpdate(const string &_strSql)
{
    if (NULL == m_statement)
    {
		init();
	}

    m_statement->setAutoCommit(true);

    TimerClock time_clock;

    PLOG_INFO("executeUpdate:[%s]", _strSql.c_str());

    unsigned int record_count = m_statement->executeUpdate(_strSql);

    PLOG_INFO("executeUpdate:count=[%u], Elapsed %d(ms)", record_count, time_clock.Elapsed());

	if (record_count == 0)
	{
		return 1;
	}

	return 0;
}

ResultSet *DBCommand::executeQuery(const string &_strSql)
{
    if (NULL == m_statement)
    {
		init();
	}

    TimerClock time_clock;

    PLOG_INFO("executeQuery:[%s]", _strSql.c_str());

    ResultSet *rs = m_statement->executeQuery(_strSql);

    PLOG_INFO("executeQuery:count=[%u], Elapsed %d(ms)", rs->getNumArrayRows(), time_clock.Elapsed());

	return rs;
}

void DBCommand::setAutoCommit(bool auto_commit)
{
    if (NULL == m_statement)
    {
		return;
	}

    bAutoCommit = false;

    m_statement->setAutoCommit(auto_commit);
}

void DBCommand::commit()
{
	if (NULL == m_connection)
	{
		throw DBException(DBException::INVALID_CONNECTION);
	}

	m_connection->commit();
}

void DBCommand::rollBack()
{
	if (NULL == m_connection)
	{
		throw DBException(DBException::INVALID_CONNECTION);
	}

	m_connection->rollback();
}
