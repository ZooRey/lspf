#include "myserver.h"
#include "service/data_sign.h"
#include "service/busi_server.h"
#include "common/app_conf.h"
#include "log/log.h"
#include "log/scribe_log.h"
#include "rpc/rpc.h"
#include "database/db_oracle.h"
#include "mexception.h"
#include "oldapi/xservice.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/thread.hpp>

using namespace lspf::log;
using namespace lspf::rpc;

//初始化日志服务
static bool initLog()
{
    try{
        AppConf *conf = AppConf::Instance();

        lspf::log::ScribeLog::SetHostAndPort(conf->GetConfStr("Log", "ServiceIp"),
                                             conf->GetConfInt("Log", "ServicePort"));

        lspf::log::ScribeLog::SetLogCategory(conf->GetConfStr("Log", "LogCategory"));

        lspf::log::ScribeLog::SetErrorCategory(conf->GetConfStr("Log", "ErrLogCategory"));

        lspf::log::Log::SetLogPriority(conf->GetConfStr("Log", "Priority"));


        TLogFunc tlogfunc = boost::bind(&lspf::log::ScribeLog::Write, _1, _2, _3, _4, _5, _6);

        lspf::log::Log::RegisterTlogFunc(tlogfunc, NULL);

        ScribeLog::Start();

        /*
        cout << "Log ServiceIp: " << conf->GetConfStr("Log", "ServiceIp") << endl;
        cout << "Log ServicePort: " << conf->GetConfInt("Log", "ServicePort") << endl;
        cout << "Log LogCategory: " << conf->GetConfStr("Log", "LogCategory") << endl;
        cout << "Log ErrLogCategory: " << conf->GetConfStr("Log", "ErrLogCategory") << endl;
        cout << "Log Priority: " << conf->GetConfStr("Log", "Priority") << endl;
        */

    }catch(MException &mx){
        PLOG_ERROR("call initLog Exception...:%s",mx.what());
        return false;
    }

    return true;
}

//初始化数据库连接池
static bool initDBPool()
{
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

    return true;
}
//初始化RPC客户端连接池
static bool initRpcConnPool()
{
    try{
        AppConf *conf = AppConf::Instance();

        ConnectionPoolMgr *mgr = ConnectionPoolMgr::Instance();

        mgr->CreateConnectionPool(ConnectionPoolMgr::TSOCKET,
                                  conf->GetConfStr("RickCtrlService", "ServiceName"),
                                  conf->GetConfStr("RickCtrlService", "ServiceIp"),
                                  conf->GetConfInt("RickCtrlService", "ServicePort"),
                                  conf->GetConfInt("RickCtrlService", "MinConn"),
                                  conf->GetConfInt("RickCtrlService", "MaxConn"),
                                  conf->GetConfInt("RickCtrlService", "Timeout"));

    }catch (TException& tx) {
        PLOG_ERROR("TException: %s ", tx.what());
        return false;
    }catch (MException& tx) {
        PLOG_ERROR("MException: %s ", tx.what());
        return false;
    }

    PLOG_INFO("initRpcConnPool successful");

    return true;
}

bool MyServer::init()
{
    //初始化日志服务
    initLog();

    //初始化数据库连接池
    initDBPool();

    //初始化RPC客户端连接池
    //initRpcConnPool();

    //xsvc_init_client();

    DataSign::InitEncKey(AppConf::Instance()->GetConfStr("Organize", "OrganizeCode"));

	return true;
}

//启动自定义服务
bool MyServer::start()
{
    AppConf *conf = AppConf::Instance();

    TThreadedServer server(
        boost::make_shared<AccPaymentProcessorFactory>(boost::make_shared<AccPaymentCloneFactory>()),
        boost::make_shared<TServerSocket>(conf->GetConfInt("RPCServer", "ServicePort")), //port
        boost::make_shared<TBufferedTransportFactory>(),
        boost::make_shared<TBinaryProtocolFactory>());

    PLOG_INFO("Starting the server...");

    server.serve();

    PLOG_INFO("Done.");

    return true;
}

//停止自定义服务
void MyServer::stop()
{

    DBConnManage *pDBConnManage = DBConnManage::instance();

    pDBConnManage->fini();

    xsvc_free_client();
}
