#include "zk_client.h"

#include "common/string_utility.h"
#include "log/log.h"

#include <map>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

#define ZK_MAX_CONNECT_TIMES 100


void ServiceWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctx)
{
    PLOG_INFO("type:%d, state:%d, path %s", type, state, path);
    ZkClient* zk_client = reinterpret_cast<ZkClient*>(watcher_ctx);

    if (state == ZOO_CONNECTED_STATE) {
        zk_client->SetConnState(true);
        PLOG_INFO("state: ZOO_CONNECTED_STATE");
        if (type == -1){
            PLOG_INFO("type= -1, Update all service list");
            zk_client->Update();//更新服务列表
        }else if (type == ZOO_CHILD_EVENT) {
            PLOG_INFO("type= ZOO_CHILD_EVENT, Update  service path: %s", path);
            zk_client->Update(path);//更新服务列表
        }
    } else if (state == ZOO_AUTH_FAILED_STATE) {
        PLOG_INFO("InitWatcher() ZOO_AUTH_FAILED_STATE");
    } else if (state == ZOO_EXPIRED_SESSION_STATE) {
        PLOG_ERROR("state: ZOO_EXPIRED_SESSION_STATE, need reconnect server");
        ///连接出错，重新连接
        zk_client->ConnectZK();
    } else if (state == ZOO_CONNECTING_STATE) {
        PLOG_INFO("InitWatcher() ZOO_CONNECTING_STATE");
    } else if (state == ZOO_ASSOCIATING_STATE) {
        PLOG_INFO("InitWatcher() ZOO_ASSOCIATING_STATE");
    }
}


ZkClient* ZkClient::instance_ = NULL;

ZkClient* ZkClient::Instance()
{
    if (instance_ == NULL){
        instance_ =  new ZkClient();
    }
    return instance_;
}

ZkClient::ZkClient() : m_connected(false), m_zhandle(NULL)
{

}
ZkClient::~ZkClient()
{

}

void ZkClient::Init(const std::string &zk_hosts, const std::string &service_name_list, const int timeout)
{
    m_zk_hosts = zk_hosts;
    m_timeout = timeout;

    StringUtility::Split(service_name_list, ",", &m_service_name_list);

    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN); //设置日志级别,避免出现一些其他信息

    std::vector<std::string>::iterator it = m_service_name_list.begin();
    for (; it != m_service_name_list.end(); ++it){
        ServerInfoPtr server_info(new ServerInfo);
        m_all_serviceinfo_list.insert(make_pair(*it, server_info));

        m_service_count.insert(make_pair(*it, 0));
    }
}

//连接zk server
void ZkClient::ConnectZK() {

    if (m_zhandle) {
        zookeeper_close(m_zhandle);
    }

    m_zhandle = NULL;
    m_connected = false;

    while (!m_connected) {
        PLOG_INFO("Connect zookeeper server start...");
        m_zhandle = zookeeper_init(m_zk_hosts.c_str(), ServiceWatcher, m_timeout, NULL, this, 0);
        sleep(5);
    }

    PLOG_INFO("Connect zookeeper server successful!");
}
//更新服务列表
void ZkClient::Update()
{
    std::vector<std::string>::iterator it = m_service_name_list.begin();
    for (; it != m_service_name_list.end(); ++it){
        std::string path = "/" + (*it);
        Update(path);
    }
}

//更新服务列表，冷备和热备
void ZkClient::Update(const std::string &path)
{
    if (m_zhandle == NULL || m_connected == false) {
        ConnectZK();
    }

    //获得服务份数
    struct String_vector str_vec;
    int ret = zoo_wget_children(m_zhandle, path.c_str(), ServiceWatcher, this, &str_vec);
    if (ret) {
        PLOG_ERROR("zoo_wget_children path:[%s] failed : %s", path.c_str(), zerror(ret));
        return;
    }

    ServerInfoPtr server_info = m_all_serviceinfo_list[path.substr(1)];

    AutoLocker locker(&m_mutex_server_infos);
    if (!server_info.unique())
    {
        server_info.reset(new ServerInfo);
    }

    server_info->clear();

    //获得各份服务ip:port
    for (int i = 0; i < str_vec.count; ++i) {
        ///更新服务列表
        server_info->push_back(str_vec.data[i]);
        PLOG_INFO("service[%d] info: %s", i+1, str_vec.data[i]);
    }

    m_all_serviceinfo_list[path.substr(1)] = server_info;
}


int ZkClient::GetServerInfo(const std::string &service_name, int &port, std::string &ip)
{
    std::size_t count = 0;

    if (m_all_serviceinfo_list.find(service_name) == m_all_serviceinfo_list.end()){
        PLOG_ERROR("no service, service name : %s", service_name.c_str());
        return -1;
    }

	ServerInfoPtr server_infos;
	{
		AutoLocker locker(&m_mutex_server_infos);
		count = ++m_service_count[service_name];
		server_infos = m_all_serviceinfo_list[service_name];
		if (count > 99999999){
            count = 0;
            m_service_count[service_name] = 0;
		}
	}

    if (server_infos->size() == 0){
        PLOG_ERROR("no record, service name : %s", service_name.c_str());
        return -1;
    }

	std::size_t idx;
	std::size_t pos;
	std::string server_info;

	idx = count % server_infos->size();

	server_info = server_infos->at(idx);

	pos = server_info.find(":");
	if (pos == std::string::npos)
	{
	    PLOG_ERROR("node info is error, server_info : %s", server_info.c_str());
		return -1;
	}

	ip = server_info.substr(0, pos);
	port = atoi(server_info.substr(pos+1).c_str());

	return 0;
}

int ZkClient::GetAllServerInfos(const std::string &service_name, std::vector<std::string> &server_info_lst)
{
    if (m_all_serviceinfo_list.find(service_name) == m_all_serviceinfo_list.end()){
        return -1;
    }

	ServerInfoPtr server_infos;
	{
		AutoLocker locker(&m_mutex_server_infos);
		server_infos = m_all_serviceinfo_list[service_name];
	}

	server_info_lst.assign(server_infos->begin(), server_infos->end());

	return 0;
}
