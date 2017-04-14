#include <time.h>
#include "common/mutex.h"
#include "common/condition_variable.h"
#include "connection_exception.h"
#include "connection_pool.h"

namespace lspf{
namespace rpc{

#define MINCONN 50
#define MAXCONN 500

ConnectionPool::ConnectionPool():
    conTimeOut(60),
    openConns(0),
    maxConns(5),
    minConns(0),
    m_mutex(new TMutex),
    m_cond_not_empty(new ConditionVariable)
{

}

ConnectionPool::~ConnectionPool()
{
    delete m_mutex;
    delete m_cond_not_empty;
}

void ConnectionPool::Init()
{
    for (int i=1; i<=minConns; ++i){
        TTransportPtr transport = CreateConnection();
        if (transport == NULL)
        {
            continue;
        }

        ConnService connService_;
        connService_.use_time = time(NULL);
        connService_.conn = transport;

        AutoLocker AutoLocker_(m_mutex);
        m_pool.push_back(connService_);
        ++openConns;
    }
}

TTransportPtr ConnectionPool::GetConnection(int timeout_in_ms)
{
    time_t now = time(NULL);
    ConnService connSerivce_;

    AutoLocker AutoLocker_(m_mutex);

    while (!m_pool.empty())
    {
        connSerivce_ = m_pool.front();
        m_pool.pop_front();

        if (!(connSerivce_.conn)->isOpen())
        {
            //连接已被关闭的
            continue;
        }

        if (openConns > minConns && now - connSerivce_.use_time > conTimeOut)
        {
            //清除已超时的连接后打开连接数-1
            --openConns;
            continue;
        }

        return connSerivce_.conn;
    }

    if (openConns < maxConns)
    {
        TTransportPtr transport = CreateConnection();
        if (transport != NULL)
        {
            ++openConns;
        }

        return transport;
    }

    //条件等待
    m_cond_not_empty->TimedWait(m_mutex, timeout_in_ms);
    if (m_pool.empty())
    {
        //超时未获取到连接
        throw(ConnectionException(ConnectionException::NON_ENOUGH_CONNECTION));
    }

    connSerivce_ = m_pool.front();
    m_pool.pop_front();

    return connSerivce_.conn;
}

void ConnectionPool::TerminateConnection(TTransportPtr connection)
{
    connection->close();

    AutoLocker AutoLocker_(m_mutex);
    --openConns;
}

void ConnectionPool::ReleaseConnection(TTransportPtr connection)
{
    if (connection == NULL)
    {
        return;
    }

    ConnService connService_;

    connService_.use_time = time(NULL);
    connService_.conn = connection;

    AutoLocker AutoLocker_(m_mutex);
    m_pool.push_back(connService_);

    //条件通知
    m_cond_not_empty->Broadcast();
}

void ConnectionPool::SetTimeOut(unsigned int connTimeOut)
{
    conTimeOut = connTimeOut;
}

void ConnectionPool::SetPoolSize(unsigned int minConn, unsigned int maxConn)
{
    minConns = minConn > MINCONN ? 1 : minConn;
    maxConns = maxConn > MAXCONN ? 100 : maxConn;
}

}
}

