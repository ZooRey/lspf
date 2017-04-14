#include "connection_exception.h"
#include "tcp_connection_pool.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <thrift/thrift.h>
#include <thrift/transport/TTransport.h>
#include <thrift/transport/TSocket.h>

using namespace apache::thrift;
using namespace apache::thrift::transport;


namespace lspf{
namespace rpc{

TcpConnectionPool::TcpConnectionPool(const std::string ip, int port, int timeout_sec)
    :m_serviceIP(ip), m_servicePort(port), m_timeout(timeout_sec)
{

}

TcpConnectionPool::~TcpConnectionPool()
{

}

TTransportPtr TcpConnectionPool::CreateConnection()
{
    boost::shared_ptr<TSocket> socket(new TSocket(m_serviceIP, m_servicePort));

    try{
        if (m_timeout>30 && m_timeout <5)
        {
            //设置最大通讯时间为30秒
            m_timeout = 30;
        }

        socket->setConnTimeout(m_timeout);

        socket->setKeepAlive(true);

        socket->open();

    }catch(TException &tx){
        throw(ConnectionException(ConnectionException::CONNECT_FAILED));
    }

    return socket;
}

}//namespace rpc
}//namespace lspf
