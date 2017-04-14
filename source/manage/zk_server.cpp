#include "zk_server.h"

#include "common/string_utility.h"
#include "log/log.h"

#include <map>
#include <string>
#include <vector>
#include <iostream>

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

    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN); //设置日志级别,避免出现一些其他信息
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
        sleep(5);
    }

    PLOG_INFO("Connect zookeeper server successful!");
}

//发布服务，建立临时节点
void ZkServer::Register()
{
    PLOG_INFO("zookeeper Register start...");

    string server_path = "/" + m_service_name + "/" + m_data;
    while(1){
        char res_path[128];
        int rc = zoo_create(m_zhandle, server_path.c_str(), "", 0,
                &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, res_path, 128);

        if (rc) {
            PLOG_INFO("zoo_create() path=%s, error=%s", server_path.c_str(), zerror(rc));
            sleep(5);
        }
        break;
    }

    PLOG_INFO("zookeeper Register succeed...");
}


