#include "zoo_keeper_client.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

ZooKeeperClient* ZooKeeperClient::instance_ = NULL;

ZooKeeperClient* ZooKeeperClient::Instance()
{
    if (instance_ == NULL){
        instance_ =  new ZooKeeperClient();
    }
    return instance_;
}

//��zookeeper_init������watcher, ��zookeeper client��server�Ự������, ����watcher
//��watcher��state=3(ZOO_CONNECTED_STATE),type=-1(ZOO_SESSION_EVENT)ʱ,ȷ�ϻỰ�ɹ�����,
//��ʱzookeeper client ��ʼ���ɹ����ɽ��к���������

//��watcher���Ӳ���zk��,watcher���յ�һ��type=-1,state=1(ZOO_CONNECTING_STATE)���¼�,
//ֱ��watcher������zk��,watcher���յ�һ��type=-1,state=-112(ZOO_EXPIRED_SESSION_STATE)���¼�,
//���ȷ��������,��watcher���ٴ��յ�һ��type=-1,state=3���¼���
void watcher_global(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctx)
{
	ZooKeeperClient* zk_client = reinterpret_cast<ZooKeeperClient*>(watcher_ctx);
	/*state=-112 �Ự��ʱ״̬ ZOO_EXPIRED_SESSION_STATE
	state= -113����֤ʧ��״̬ ZOO_AUTH_FAILED_STATE
	state= 1 ���ӽ����� ZOO_CONNECTING_STATE
	state= 2 (��ʱ��������������״̬,ZOO_ASSOCIATING_STATE)
	state=3 �����ѽ���״̬ ZOO_CONNECTED_STATE
	state= 999 ������״̬
	*/
	if (ZOO_SESSION_EVENT == type)
    {
		cout << "state = " << state << endl;
		if (ZOO_CONNECTED_STATE == state)
		{
			zk_client->SetConnStat(0);
		}
    }	
}

void watcher_get_children(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctx)
{
	cout << "watcher_get_children, ZOO_CHILD_EVENT=" << ZOO_CHILD_EVENT << ", type=" << type << ", state=" << state << endl;
	ZooKeeperClient* zk_client = reinterpret_cast<ZooKeeperClient*>(watcher_ctx);
	
	if (state == ZOO_CONNECTED_STATE) //ZOO_CONNECTED_STATE(3)
	{
		 if (type == ZOO_CHILD_EVENT)  //ZOO_CHILD_EVENT(4)
		 {
			 zk_client->GetChildren();
		 }
	}
}

void ZooKeeperClient::post(const String_vector &strings)
{
	int i;
	int ret;
	int buffer_len = 32;
	char buffer[32+1];
	
	AutoLocker locker(&m_mutex_server_infos);
	//if (!m_server_infos.unique())
	{
		//m_server_infos.reset(new ServerInfoList(*m_server_infos));
		m_server_infos->clear();
	}
	//String_vector ����
	//struct String_vector {
	//	int32_t count;
	//	char * *data;
	//}
	//ZOOAPI int zoo_get(zhandle_t * zh, const char *path, int watch,
    //               char *buffer, int *buffer_len, struct Stat *stat);	
	for(i=0; i<strings.count; i++)
	{
		cout << strings.data[i] << endl;
		memset(buffer, 0, sizeof(buffer));
		ret = zoo_get(m_zk_handle, strings.data[i], 0, buffer, &buffer_len, NULL);
		if (ret == ZOK)
		{
			cout << string(buffer, buffer_len) << endl;
			m_server_infos->push_back(string(buffer, buffer_len));
		}
	}		
}

int ZooKeeperClient::GetChildren()
{
	int ret;
	string path;
	String_vector strings;
	Stat stat;
	
	path = "/" + m_service_name;	
	//ZOOAPI int zoo_wget_children2(zhandle_t * zh, const char *path,
    //                          watcher_fn watcher, void *watcherCtx,
    //                          struct String_vector *strings,
    //                          struct Stat *stat);	
	ret = zoo_wget_children2(m_zk_handle, path.c_str(), watcher_get_children, this, &strings, &stat);
	if (ret != ZOK)
	{
		return ret;
	}

	post(strings);
	
	deallocate_String_vector(&strings);
	deallocate_Stat(&stat);
	return ret;
}

void ZooKeeperClient::Init(int timeout, const string &service_name, const string &zk_hosts)
{
	m_timeout = timeout;
	m_service_name = service_name;
	m_zk_hosts = zk_hosts;
	
	m_conn_stat = -1;
	
	m_trans_count = 0;

	m_zk_handle = NULL;

	//ZOO_LOG_LEVEL_DEBUG   ZOO_LOG_LEVEL_INFO   ZOO_LOG_LEVEL_WARN   ZOO_LOG_LEVEL_ERROR
	zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);
}

int ZooKeeperClient::Connect()
{	
/* 	if (NULL != m_zk_handle)
    {
        // ���ӶϿ����Ȩʧ�ܲ���Ҫ�������ӣ�����״̬���Ӷ���Ч������������
        int state = zoo_state(m_zk_handle);
        if (ZOO_EXPIRED_SESSION_STATE != state && ZOO_AUTH_FAILED_STATE != state)
        {
            return 0;
        }
        Close();
    } */
	
	//ZOOAPI zhandle_t *zookeeper_init(const char *host, watcher_fn fn,int recv_timeout,const clientid_t * clientid,void *context, int flags);
	m_zk_handle = zookeeper_init(m_zk_hosts.c_str(), watcher_global, m_timeout, 0, this, 0);
    if (NULL == m_zk_handle)
    {
        return ZNOTHING;
    }
	
	return GetConnStat();
}

int ZooKeeperClient::Close()
{
	return zookeeper_close(m_zk_handle);
}
	
int ZooKeeperClient::Register(const string &node_name, const string &data)
{
	int ret;
	char c_path[32+1] = {0};
	string path;
	
	path = "/" + m_service_name + "/" + node_name;
	//ZOOAPI int zoo_create(zhandle_t * zh, const char *path,
    //                  const char *value, int valuelen,
    //                  const struct ACL_vector *acl, int flags,
    //                  char *path_buffer, int path_buffer_len);
	
	ret = zoo_create(m_zk_handle, path.c_str(), data.c_str(), data.size(), 
				&ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, c_path, 32);
	cout << "zoo_create return " << ret << endl;
	return ret;
}	

int ZooKeeperClient::GetServerInfo(int &port, string &ip)
{
	SILPtr server_infos;
	{
		AutoLocker locker(&m_mutex_server_infos);
		m_trans_count ++;
		server_infos = m_server_infos;
	}
	
	int idx;
	size_t pos;
	string server_info;
	vector<string> server_info_lst;
	
	server_info_lst.assign(server_infos->begin(), server_infos->end());
	if (server_info_lst.size() == 0)
	{
		return -1;
	}
	idx = m_trans_count % server_info_lst.size();
	server_info = server_info_lst[idx];
	pos = server_info.find(":");
	if (pos == string::npos)
	{
		return -1;
	}
	ip = server_info.substr(0, pos);
	port = atoi(server_info.substr(pos+1).c_str());
	cout << "ip=" << ip << ", port=" << port << endl;
	
	return 0;
}
int ZooKeeperClient::GetAllServerInfos(vector<string> &server_info_lst)
{
	SILPtr server_infos;
	{
		AutoLocker locker(&m_mutex_server_infos);
		server_infos = m_server_infos;
	}
	server_info_lst.assign(server_infos->begin(), server_infos->end());
	return 0;
}
