#include "connection_pool.h"
#include "tcp_connection_pool.h"
#include "connection_exception.h"
#include "connection_pool_mgr.h"

#include <map>
#include <string>
#include <thrift/transport/TTransport.h>

namespace lspf{
namespace rpc{

ConnectionPoolMgr *ConnectionPoolMgr::instance_ = NULL;

ConnectionPoolMgr::ConnectionPoolMgr()
{

}
ConnectionPoolMgr::~ConnectionPoolMgr()
{

}

ConnectionPoolMgr* ConnectionPoolMgr::Instance()
{
    if (instance_ == NULL)
    {
        instance_ = new ConnectionPoolMgr();
    }

    return instance_;
}

bool ConnectionPoolMgr::CreateConnectionPool(TTransportType type, const std::string &poolName, const std::string &ip, int port, int minConn, int maxConn, int timeout)
{
    if (poolName.empty() || connectionPoolMap.find(poolName) != connectionPoolMap.end())
    {
        throw(ConnectionException(ConnectionException::INVALID_POOL_NAME));
    }

    ConnectionPool *pool = NULL;

    switch (type){
    case TSOCKET:
        pool = new TcpConnectionPool(ip, port, timeout);
        break;
    default:
        throw(ConnectionException(ConnectionException::UNKNOWN_CONNECTION_TYPE));
    }

    if (pool == NULL)
    {
        throw(ConnectionException(ConnectionException::INTERNAL_ERROR));
    }

    pool->SetTimeOut(timeout);

    pool->SetPoolSize(minConn, maxConn);

    pool->Init();

    connectionPoolMap.insert(make_pair(poolName, pool));

    return true;
}

ConnectionPool* ConnectionPoolMgr::GetConnectionPool(const std::string &poolName)
{
    if (poolName.empty() || connectionPoolMap.find(poolName) == connectionPoolMap.end())
    {
        throw(ConnectionException(ConnectionException::INVALID_POOL_NAME));
    }

    ConnectionPool *pool = connectionPoolMap[poolName];
    if (pool == NULL)
    {
        throw(ConnectionException(ConnectionException::BAD_CONNECTION_POOL));
    }

    return pool;
}

}
}
