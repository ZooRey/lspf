#ifndef LSPF_RPC_CONNECTION_POOL_H
#define LSPF_RPC_CONNECTION_POOL_H

#include <time.h>
#include <list>
#include <boost/noncopyable.hpp>
#include <thrift/transport/TTransport.h>

using namespace apache::thrift::transport;

class TMutex;
class ConditionVariable;

typedef boost::shared_ptr<TTransport> TTransportPtr;

namespace lspf{
namespace rpc{

class ConnectionPool : public boost::noncopyable
{
public :
    ConnectionPool();
	virtual ~ConnectionPool();

    void Init();

    virtual TTransportPtr CreateConnection() = 0;

	TTransportPtr GetConnection(int timeout_in_ms = 1000);

    void TerminateConnection(TTransportPtr connection);

	void ReleaseConnection(TTransportPtr connection);

    void SetTimeOut(unsigned int connTimeOut =0);

    void SetPoolSize(unsigned int minConn =0, unsigned int maxConn =1);

	unsigned int GetOpenConnections() const{
        return openConns;
	}

	unsigned int GetMinConnections() const{
        return minConns;
	}

	unsigned int GetMaxConnections() const{
        return maxConns;
	}

	unsigned int GetTimeOut() const{
        return conTimeOut;
	}

protected:
    /** 连接超时配置 */
    int conTimeOut;

    /** 已打开连接数 */
    int openConns;

    /** 最大连接数 */
    int maxConns;

    /** 最小连接数 */
    int minConns;

    TMutex *m_mutex;

    ConditionVariable *m_cond_not_empty;

    struct ConnService{
        time_t use_time;
        TTransportPtr conn;
    };

    std::list<ConnService> m_pool;
};

}
}


#endif

