#ifndef ZK_CLIENT_H
#define ZK_CLIENT_H

#include "common/mutex.h"
#include "common/condition_variable.h"
#include <zookeeper/zookeeper.h>

#include <map>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

class ZkClient
{
public:
    static ZkClient* Instance();

    void Init(const std::string &zk_hosts, const std::string &service_name, const int timeout);

    //连接zk server
    void ConnectZK();

    //更新服务列表
    void Update();

    void Update(const std::string &path);

    int GetServerInfo(const std::string &service_name, int &port, std::string &ip);

    int GetAllServerInfos(const std::string &service_name, std::vector<std::string> &server_info_lst);

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
    ZkClient();
    ~ZkClient();

private:
    bool m_connected;
    zhandle_t *m_zhandle;

    std::string m_zk_hosts;
    int m_timeout;

    TMutex m_mutex_server_infos;
	TMutex m_mutex_conn_state;
	ConditionVariable m_condi_conn_state;
    typedef std::vector<std::string> ServerInfo;
    typedef boost::shared_ptr<std::vector<std::string> > ServerInfoPtr;

    std::vector<std::string> m_service_name_list;
    std::map<std::string, ServerInfoPtr> m_all_serviceinfo_list;

    std::map<std::string, std::size_t> m_service_count;

    static ZkClient* instance_;
};


#endif // ZK_CLIENT_H
