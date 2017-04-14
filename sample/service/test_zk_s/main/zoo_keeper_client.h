#ifndef __ZOO_KEEPER_CLIENT_H__
#define __ZOO_KEEPER_CLIENT_H__

#include "common/mutex.h"
#include "common/condition_variable.h"
#include <zookeeper/zookeeper.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
using namespace std;

class ZooKeeperClient
{
public:
	static ZooKeeperClient* Instance();
public:
	void Init(int timeout, const string &service_name, const string &zk_hosts);
	int Connect();
	int Close();
	int Register(const string &node_name, const string &data);
	int GetChildren();
	
	int GetServerInfo(int &port, string &ip);
	int GetAllServerInfos(vector<string> &server_info_lst);
	
	int GetConnStat()
	{
		AutoLocker locker(&m_mutex_conn_stat);
		while (m_conn_stat == -1)
		{
			m_condi_conn_stat.TimedWait(&m_mutex_conn_stat, m_timeout);			
		}
		return m_conn_stat;	
	}
	void SetConnStat(int conn_stat)
	{
		{
			AutoLocker locker(&m_mutex_conn_stat);
			m_conn_stat = conn_stat;
		}
		m_condi_conn_stat.Signal();		
	}	
private:
	ZooKeeperClient() 
	:m_server_infos(new ServerInfoList())	
	{
		
	}

	void post(const String_vector &strings);
private:
	typedef vector<string> ServerInfoList;
	typedef boost::shared_ptr<ServerInfoList> SILPtr;
	
	int m_timeout;
	string m_service_name;
	string m_zk_hosts;
	
	int m_conn_stat;	
	TMutex m_mutex_conn_stat;
	ConditionVariable m_condi_conn_stat;	

	int m_trans_count;	
	SILPtr m_server_infos;
	TMutex m_mutex_server_infos;	
	
	zhandle_t* m_zk_handle;

	static ZooKeeperClient* instance_;
};

#endif //__ZOO_KEEPER_CLIENT_H__
