#ifndef LSPF_RPC_CONNECTION_POOL_MGR_H
#define LSPF_RPC_CONNECTION_POOL_MGR_H

#include <map>
#include <string>
#include <thrift/transport/TTransport.h>

using namespace apache::thrift::transport;

namespace lspf{
namespace rpc{

class ConnectionPool;

class ConnectionPoolMgr
{
public:
    /**
    * 传输协议类型定义.
    */
    enum TTransportType {
        UNKNOWN = 0,
        TSOCKET = 1,
        TSSLSOCKET = 2
    };

    ~ConnectionPoolMgr();

    static ConnectionPoolMgr* Instance();

    ///@brief 创建thrift连接池
    ///@param type-通讯连接类型  poolName-连接池名称
    bool CreateConnectionPool(TTransportType type, const std::string &poolName, const std::string &ip, int port, int minConn, int maxConn, int timeout = 60);

    ///@brief 获取thrift连接池
    ConnectionPool* GetConnectionPool(const std::string &poolName);

protected:
    ConnectionPoolMgr();

private:
    std::map<std::string, ConnectionPool*> connectionPoolMap;

    static ConnectionPoolMgr *instance_;
};

}
}

#endif // LSPF_RPC_CONNECTION_POOL_MGR_H
