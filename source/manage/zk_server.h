#ifndef ZK_SERVER_H
#define ZK_SERVER_H


#include "common/mutex.h"
#include "common/condition_variable.h"
#include <zookeeper/zookeeper.h>

#include <map>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

class ZkServer
{
public:
    static ZkServer* Instance();

    ///初始化
    void Init(const std::string &zk_hosts, const int timeout);

    ///发布服务
    void Publish();

    ///设置服务器信息
    void SetServiceInfo(const std::string &service_name, const std::string &node_name);

    ///注册服务
    void Register();
	 
    //创建结点
    void ZooCreate();

	bool GetConnState()
	{
		AutoLocker locker(&m_mutex_conn_state);
		while (!m_connected)
		{
			m_condi_conn_state.TimedWait(&m_mutex_conn_state, m_timeout);
		}
		return m_connected;
	}

	void SetConnState(bool state)
	{
		{
			AutoLocker locker(&m_mutex_conn_state);
			m_connected = state;
		}
		m_condi_conn_state.Signal();
	}

private:
    ZkServer();
    ~ZkServer();

private:
    bool m_connected;
    zhandle_t *m_zhandle;

    std::string m_zk_hosts;
    std::string m_service_name;
    int m_timeout;

    std::string m_data;
	TMutex m_mutex_conn_state;
	ConditionVariable m_condi_conn_state;

    static ZkServer* instance_;
};


#endif // ZK_SERVER_H
