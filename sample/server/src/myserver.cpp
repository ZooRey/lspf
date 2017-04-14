#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/thread.hpp>

#include "myserver.h"
#include "demo_server.h"

#include "common/app_conf.h"
#include "common/log.h"
#include "rpc/rpc.h"
#include "database/db_oracle.h"
#include "mexception.h"


using namespace lspf::rpc;
//初始化日志服务
static bool initLog()
{
    try{
        AppConf *conf = AppConf::Instance();

        ConnectionPoolMgr *mgr = ConnectionPoolMgr::Instance();

        mgr->CreateConnectionPool(ConnectionPoolMgr::TSOCKET,
                                  conf->GetConfStr("Log", "ServiceName"),
                                  conf->GetConfStr("Log", "ServiceIp"),
                                  conf->GetConfInt("Log", "ServicePort"),
                                  conf->GetConfInt("Log", "MinConn"),
                                  conf->GetConfInt("Log", "MaxConn"),
                                  conf->GetConfInt("Log", "Timeout"));

        lspf::rpc::ScribeLog::SetPoolName(conf->GetConfStr("Log", "ServiceName"));

        lspf::rpc::ScribeLog::SetLogCategory(conf->GetConfStr("Log", "LogCategory"));

        lspf::rpc::ScribeLog::SetErrorCategory(conf->GetConfStr("Log", "ErrLogCategory"));

        TLogFunc tlogfunc = boost::bind(&lspf::rpc::ScribeLog::Write, _1, _2, _3, _4, _5, _6, _7);

        Log::RegisterTlogFunc(tlogfunc);

        Log::SetLogPriority(conf->GetConfStr("Log", "Priority"));

    }catch(MException &mx){
        return false;
    }
    return true;
}

//初始化数据库连接池
static bool initDBPool()
{
    try{
        AppConf *conf = AppConf::Instance();

        DBConnManage *pDBConnManage = DBConnManage::instance();

        pDBConnManage->init();

        if (!pDBConnManage->creatConnectionPool(conf->GetConfStr("Database", "DBName"),
                                                conf->GetConfStr("Database", "UserName"),
                                                conf->GetConfStr("Database", "Passwd"),
                                                conf->GetConfInt("Database", "MinConn"),
                                                conf->GetConfInt("Database", "MaxConn")))
        {
            return false;
        }

    }catch (MException& tx) {
        cout << "creatConnectionPool ERROR: " << tx.what() << endl;
        return false;
    }

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

    }catch (MException& tx) {
        cout << "initPool: " << tx.what() << endl;
        return false;
    }

    return true;
}

bool MyServer::init()
{
    //初始化日志服务
    initLog();

    //初始化数据库连接池
    initDBPool();

    //初始化RPC客户端连接池
    initRpcConnPool();

	return true;
}

//启动自定义服务
bool MyServer::start()
{
    AppConf *conf = AppConf::Instance();

    TThreadedServer server(
        boost::make_shared<DemoProcessorFactory>(boost::make_shared<DemoCloneFactory>()),
        boost::make_shared<TServerSocket>(conf->GetConfInt("RPCServer", "ServicePort")), //port
        boost::make_shared<TBufferedTransportFactory>(),
        boost::make_shared<TBinaryProtocolFactory>());

    std::cout << "Starting the server..." << std::endl;

    server.serve();

    std::cout << "Done." << std::endl;

    return true;
}

//停止自定义服务
void MyServer::stop()
{
    DBConnManage *pDBConnManage = DBConnManage::instance();

    pDBConnManage->fini();
}
