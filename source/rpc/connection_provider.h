#ifndef LSPF_RPC_CONNECTION_PROVIDER_H
#define LSPF_RPC_CONNECTION_PROVIDER_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <thrift/transport/TTransport.h>

using namespace apache::thrift::transport;

typedef boost::shared_ptr<TTransport> TTransportPtr;

namespace lspf{
namespace rpc{

class ConnectionPool;

class ConnectionProvider : public boost::noncopyable
{
public:
    ConnectionProvider(const std::string &name);

    ~ConnectionProvider();

    TTransportPtr GetTransport();

protected:
    std::string m_name;
    ConnectionPool *m_connPool;
    TTransportPtr m_transport;
};

}
}


#endif
