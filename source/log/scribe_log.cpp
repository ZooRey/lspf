#include "scribe_log.h"
#include "scribe_log_handler.h"
#include "log.h"
#include "common/thread_local.h"
#include "async_logging.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <string>

#include <boost/shared_ptr.hpp>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

namespace lspf {
namespace log {


std::string ScribeLog::m_address = "";
int ScribeLog::m_port = 0;

ScribeLog::InnerThread ScribeLog::m_thread;

/// @brief 获取当前时间戳，按照固定格式返回
static std::string GetTimeStamp()
{
    char buff[64] = {0};
    struct timeval tv_now;
    time_t now;
    struct tm tm_now;
    struct tm* p_tm_now;

    gettimeofday(&tv_now, NULL);
    now = (time_t)tv_now.tv_sec;
    p_tm_now = localtime_r(&now, &tm_now);

    snprintf(buff, sizeof(buff), "%04d%02d%02d %02d:%02d:%02d.%06d",
        1900 + p_tm_now->tm_year,
        p_tm_now->tm_mon + 1,
        p_tm_now->tm_mday,
        p_tm_now->tm_hour,
        p_tm_now->tm_min,
        p_tm_now->tm_sec,
        static_cast<int>(tv_now.tv_usec));

    return buff;
}


void ScribeLog::SetHostAndPort(const std::string &address, const int port)
{
    ScribeLogHandler::service_address = address;
    ScribeLogHandler::service_port = port;
}

void ScribeLog::SetLogCategory(const std::string &category)
{
    if (category.empty() || category.size() > 20) {
        return;
    }

    ScribeLogHandler::SetLogCategory(category);
}

void ScribeLog::SetErrorCategory(const std::string &category)
{
    if (category.empty() || category.size() > 20) {
        return;
    }

    ScribeLogHandler::SetErrorCategory(category);
}


void ScribeLog::Flush()
{
    //ThreadLocal<ScribeLogHandler>::Instance()->WriteLog();
}

#include <sys/syscall.h> /*此头必须带上*/

static pid_t gettid()
{
     return syscall(SYS_gettid);  /*这才是内涵*/
}


void ScribeLog::Write(int pri, const char* file, uint32_t line,
				  const char* function, const char *id, const char* msg)
{
    char buff[4096] = {0};

    static const char* priority_str[] = { "TRACE", "DEBUG", "INFO", "ERROR", "FATAL" };

    // log前缀，tlog时不用组装
    snprintf(buff, sizeof(buff), "[%s][%d][(%s)(%s:%u)(%s)][%s][%s] ",
            GetTimeStamp().c_str(),
            gettid(),
            Log::GetSelfName(),
            file,
            line,
            function,
            priority_str[pri],
            id
        );

    strcat(buff, msg);

    AsyncLogging::Instance()->Append(buff);
}

void ScribeLog::Start()
{
    //启动写日志线程
    m_thread.Start();
}

ScribeLog::InnerThread::InnerThread()
{

}

ScribeLog::InnerThread::~InnerThread()
{

}

void ScribeLog::InnerThread::Run()
{
    ScribeLogHandler::WriteLog();
}

} // namespace log
} // namespace lspf
