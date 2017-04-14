#include "dbconn_check.h"
#include "dbcommand.h"
#include "dbexception.h"
#include "log/log.h"
#include <occi/occi.h>

DBConnCheckThread* DBConnCheckThread::instance_ = NULL;


int DBConnCheckThread::pool_size = 1;

DBConnCheckThread::DBConnCheckThread()
{

}
DBConnCheckThread::~DBConnCheckThread()
{

}

void DBConnCheckThread::Init(const int pool_size_)
{
    pool_size = pool_size_;

    if (instance_ == NULL){
        instance_ =  new DBConnCheckThread();
    }
}

DBConnCheckThread* DBConnCheckThread::Instance()
{
    if (instance_ == NULL){
        instance_ =  new DBConnCheckThread();
    }

    return instance_;
}

void DBConnCheckThread::Run()
{
    sleep(5);

    while(true){
        PLOG_INFO("DBConnCheckThread start");
        try{
            for (int i=0; i<pool_size; ++i){
                DBCommand cmd(i);
                cmd.init();
                cmd.terminate();
            }
        }catch(SQLException &sx){
            PLOG_ERROR("SQLException %d:%s", sx.getErrorCode(), sx.getMessage().c_str());
        }catch(DBException &dx){
            PLOG_ERROR("DBException :%s", dx.what());
        }
        PLOG_INFO("DBConnCheckThread end");

        sleep(10);
    }
}
