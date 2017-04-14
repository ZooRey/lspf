#ifndef LSPF_RPC_TCPCONNECTION_POOL_H
#define LSPF_RPC_TCPCONNECTION_POOL_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <thrift/transport/TTransport.h>
#include "connection_pool.h"

using namespace apache::thrift::transport;

typedef boost::shared_ptr<TTransport> TTransportPtr;

namespace lspf{
namespace rpc{

class TcpConnectionPool : public ConnectionPool
{
public:
    TcpConnectionPool(const std::string ip, int port, int timeout_sec = 60);

	virtual ~TcpConnectionPool();

    virtual TTransportPtr CreateConnection();

private:
    /** ����IP��ַ */
    std::string m_serviceIP;

    /** ����˿� */
    int m_servicePort;

    int m_timeout;
};

}
}
#endif // LSPF_RPC_TCPCONNECTION_POOL_H
