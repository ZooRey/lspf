#include "myserver.h"
#include "zoo_keeper_client.h"
#include "trade_server.h"
#include "common/app_conf.h"
#include "common/thread_pool.h"
#include "common/string_utility.h"
#include "log/log.h"
#include "log/scribe_log.h"
#include "database/db_oracle.h"
#include "mexception.h"
#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/thread.hpp>

using namespace std;
using namespace lspf::log;
//using namespace lspf::rpc;

//初始化日志服务
static bool initLog()
{
/*
    try{
        AppConf *conf = AppConf::Instance();

        lspf::log::ScribeLog::SetHostAndPort(conf->GetConfStr("Log", "ServiceIp"),
                                             conf->GetConfInt("Log", "ServicePort"));

        lspf::log::ScribeLog::SetLogCategory(conf->GetConfStr("Log", "LogCategory"));

        lspf::log::ScribeLog::SetErrorCategory(conf->GetConfStr("Log", "ErrLogCategory"));

        lspf::log::Log::SetLogPriority(conf->GetConfStr("Log", "Priority"));

        //TLogFunc tlogfunc = boost::bind(&lspf::log::ScribeLog::Write, _1, _2, _3, _4, _5, _6, _7);

        //Log::RegisterTlogFunc(tlogfunc, boost::bind(&lspf::log::ScribeLog::Flush));

		PLOG_INFO("Register log function succeed");
        //cout << "Log ServiceIp: " << conf->GetConfStr("Log", "ServiceIp") << endl;
        //cout << "Log ServicePort: " << conf->GetConfInt("Log", "ServicePort") << endl;
        //cout << "Log LogCategory: " << conf->GetConfStr("Log", "LogCategory") << endl;
        //cout << "Log ErrLogCategory: " << conf->GetConfStr("Log", "ErrLogCategory") << endl;
        //cout << "Log Priority: " << conf->GetConfStr("Log", "Priority") << endl;

    }catch(MException &mx){
        cout << "call initLog Exception..." << mx.what() << endl;
        return false;
    }
*/
    return true;
}

//初始化数据库连接池
static bool initDBPool()
{
	/*
    try{
        AppConf *conf = AppConf::Instance();

        std::string db_name = conf->GetConfStr("Database", "DBName");
        std::string user_name = conf->GetConfStr("Database", "UserName");
        std::string passwd = conf->GetConfStr("Database", "Passwd");
        int min_conn = conf->GetConfInt("Database", "MinConn");
        int max_conn = conf->GetConfInt("Database", "MaxConn");

        DBConnManage *pDBConnManage = DBConnManage::instance();

        pDBConnManage->init();

        if (user_name.empty() || passwd.empty()){
            PLOG_ERROR("database parameter error");
            return false;
        }

        if (min_conn <= 0){
            min_conn = 1;
        }

        if (max_conn < 1){
            max_conn = 20;
        }

        if (!pDBConnManage->creatConnectionPool(db_name, user_name, passwd, min_conn, max_conn)){
            PLOG_ERROR("create database connectionPool failed");
            return false;
        }

    }catch (MException& tx) {
        cout << "creatConnectionPool ERROR: " << tx.what() << endl;
        PLOG_ERROR("create database connectionPool failed");
        return false;
    }

    PLOG_INFO("create database connectionPool succeed");
	*/
    return true;
}

bool MyServer::init(int port)
{
    //初始化日志服务
    initLog();
    //初始化数据库连接池
    initDBPool();

	ZooKeeperClient::Instance()->Init(3000, "test_zk_s", "127.0.0.1:2181,127.0.0.1:2182,127.0.0.1:2183");
	if (ZooKeeperClient::Instance()->Connect() != 0)
	{
		return false;
	}
	char node_name[16+1] = {0};
	sprintf(node_name, "%d", port);
	string data = "127.0.0.1:" + string(node_name);
	if (ZooKeeperClient::Instance()->Register(string(node_name), data) != 0)
	{
		return false;
	}

	return true;
}

//启动自定义服务
bool MyServer::start(int port)
{
	cout << "app start..." << endl;
	//rpc_server
    TThreadedServer rpc_server(
        boost::make_shared<CupsChannelProcessorFactory>(boost::make_shared<CupsChannelCloneFactory>()),
        boost::make_shared<TServerSocket>(port), //port
        boost::make_shared<TBufferedTransportFactory>(),
        boost::make_shared<TBinaryProtocolFactory>());

	cout << "rpc service start..." << endl;
    rpc_server.serve();
	
    return true;
}

//停止自定义服务
void MyServer::stop()
{

    //DBConnManage *pDBConnManage = DBConnManage::instance();

    //pDBConnManage->fini();

    //xsvc_free_client();
}
