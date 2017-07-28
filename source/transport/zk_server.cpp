#include "zk_server.h"

#include "common/string_utility.h"
#include "log/log.h"

#include <map>
#include <string>
#include <vector>
#include <iostream>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define ZK_MAX_CONNECT_TIMES 100

void InitWatcher(zhandle_t *zh, int type, int state, const char *path, void *watcher_ctx) {

    ZkServer* zk_server = reinterpret_cast<ZkServer*>(watcher_ctx);

    PLOG_INFO("type:%d, state:%d, path %s", type, state, path);

    if (state == ZOO_CONNECTED_STATE) {
        zk_server->SetConnState(true);
        PLOG_INFO("ZOO_CONNECTED_STATE Register");
        zk_server->Register();
    } else if (state == ZOO_AUTH_FAILED_STATE) {
        PLOG_INFO("InitWatcher() ZOO_AUTH_FAILED_STATE");
    } else if (state == ZOO_EXPIRED_SESSION_STATE) {
        PLOG_ERROR("ZOO_EXPIRED_SESSION_STATE, need reconnect server and register");
        ///连接出错，重新连接注册
        zk_server->Publish();
    } else if (state == ZOO_CONNECTING_STATE) {
        zk_server->SetConnState(false);
        PLOG_INFO("InitWatcher() ZOO_CONNECTING_STATE");
    } else if (state == ZOO_ASSOCIATING_STATE) {
        PLOG_INFO("InitWatcher() ZOO_ASSOCIATING_STATE");
    }else{
        PLOG_INFO("InitWatcher() other");
    }
}

ZkServer* ZkServer::instance_ = NULL;

ZkServer* ZkServer::Instance()
{
    if (instance_ == NULL){
        instance_ =  new ZkServer();
    }
    return instance_;
}

ZkServer::ZkServer() : m_connected(false), m_zhandle(NULL)
{

}
ZkServer::~ZkServer()
{

}

void ZkServer::Init(const std::string &zk_hosts, const int timeout)
{
    m_zk_hosts = zk_hosts;
    m_timeout = timeout;
    
    //zoo_set_debug_level(ZOO_LOG_LEVEL_WARN); //设置日志级别,避免出现一些其他信息
    zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR); //设置日志级别,避免出现一些其他信息
}


void ZkServer::SetServiceInfo(const std::string &service_name, const std::string &node_name)
{
    m_service_name = service_name;

    m_data = node_name;
}


//连接zk server
void ZkServer::Publish()
{
    m_zhandle = NULL;
    m_connected = false;

    while (!m_connected){
        PLOG_INFO("Connect zookeeper server start...");
        m_zhandle = zookeeper_init(m_zk_hosts.c_str(), InitWatcher, m_timeout, NULL, this, 0);
        if(m_zhandle != NULL)
        {
            PLOG_INFO("zookeeper_init OK");
            break;
        }
        sleep(5);
    }

    PLOG_INFO("Connect zookeeper server successful!");
}


void zooCreateCallback(int rc, const char *path, const void *data)
{
	ZkServer *zk_server = (ZkServer *)data;
	if(rc == ZOK || rc == ZNODEEXISTS){
		PLOG_INFO("ZkServer create node successfully, path=%s", path);
	}else{
		PLOG_ERROR("ZkServer create node failed, path=%s, error=%s", path, zerror(rc));
		sleep(1);
		zk_server->Register();
	}
}

std::string ZkServer::GetLocalIpAddress()
{
    struct sockaddr_in local_addr;
    socklen_t local_addr_len = sizeof(local_addr);

    char local_ip[64] = {0};

    int socket_id = *(int*)m_zhandle;

    getsockname(socket_id, (struct sockaddr *)&local_addr, &local_addr_len);
    inet_ntop(AF_INET, &local_addr.sin_addr, local_ip, sizeof(local_ip));

    return std::string(local_ip);
}

//发布服务，建立临时节点
void ZkServer::Register()
{
	if(m_connected)
	{
        if(m_data.substr(0, 9) == "localhost")
        {
            m_data = GetLocalIpAddress() + m_data.substr(9);
        }

		string server_path = "/" + m_service_name + "/" + m_data;

		PLOG_ERROR("zookeeper Register start, server_path=%s", server_path.c_str());

		int rc = zoo_acreate(m_zhandle, server_path.c_str(), "", 0, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, zooCreateCallback, this);
		if(rc)
		{
			PLOG_ERROR("ZkServer acreate failed, path=%s, error=%s", server_path.c_str(), zerror(rc));
		}
	}
}
