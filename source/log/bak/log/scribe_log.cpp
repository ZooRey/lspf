#include "scribe_log.h"
#include "scribe_log_handler.h"
#include "log.h"
#include "common/thread_local.h"

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

static std::string g_log_category = "log";
static std::string g_error_category = "error";

std::string ScribeLog::m_address = "";
int ScribeLog::m_port = 0;

/// @brief 获取当前时间戳，按照固定格式返回
static const char* GetTimeStamp()
{
    static char buff[64] = {0};
    static struct timeval tv_now;
    static time_t now;
    static struct tm tm_now;
    static struct tm* p_tm_now;

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

    g_log_category = category;
}

void ScribeLog::SetErrorCategory(const std::string &category)
{
    if (category.empty() || category.size() > 20) {
        return;
    }

    g_error_category = category;
}

void ScribeLog::Flush()
{
    ThreadLocal<ScribeLogHandler>::Instance()->WriteLog();
}

void ScribeLog::Write(int pri, const char* file, uint32_t line,
				  const char* function, uint32_t id, uint32_t cls, const char* msg)
{
    char buff[4096] = {0};

    static const char* priority_str[] = { "TRACE", "DEBUG", "INFO", "ERROR", "FATAL" };

    // log前缀，tlog时不用组装
    snprintf(buff, sizeof(buff), "[%s][%ld][(%s:%d)(%s)][%s][%u] ",
            GetTimeStamp(),
            pthread_self(),
            file,
            line,
            function,
            priority_str[pri],
            id
        );

    strcat(buff, msg);

    ScribeLogHandler* log = ThreadLocal<ScribeLogHandler>::Instance();
    // 输出到ERROR文件
    if (pri >= LOG_PRIORITY_ERROR) {
        log->AddErrorLog(g_error_category, buff);
    }

    // 输出到log文件
    log->AddLog(g_log_category, buff);
}

} // namespace log
} // namespace lspf
