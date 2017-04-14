#include "connection_pool.h"
#include "connection_pool_mgr.h"
#include "connection_provider.h"
#include "connection_exception.h"

#include <thrift/thrift.h>
#include <thrift/transport/TTransport.h>

using namespace apache::thrift;
using namespace apache::thrift::transport;

namespace lspf{
namespace rpc{

ConnectionProvider::ConnectionProvider(const std::string &name)
    : m_name(name), m_connPool(NULL), m_transport()
{

}

ConnectionProvider::~ConnectionProvider()
{
    try{
        if (m_transport == NULL || m_connPool == NULL)
        {
            return;
        }

        if (m_transport->isOpen())
        {
            m_connPool->ReleaseConnection(m_transport);
        }
        else
        {
            m_connPool->TerminateConnection(m_transport);
        }
    }catch(TException& tx){
        //不处理异常
    }
}

TTransportPtr ConnectionProvider::GetTransport()
{
    m_connPool = ConnectionPoolMgr::Instance()->GetConnectionPool(m_name);
    if (m_connPool){
        m_transport = m_connPool->GetConnection();
    }
    else{
        throw(ConnectionException(ConnectionException::INVALID_POOL_NAME));
    }

    return m_transport;
}

}
}


