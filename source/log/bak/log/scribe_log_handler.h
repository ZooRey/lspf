#ifndef LSPF_SCLOG_HANDLER_H
#define LSPF_SCLOG_HANDLER_H

#include <vector>
#include <string>

#include "gen-cpp/scribe.h"

using namespace scribe::thrift;

namespace lspf {
namespace log {

//暂仅支持线程池收集日志，不支持不断新建销毁线程记录日志
class ScribeLogHandler {
public:
	ScribeLogHandler(){}
    ~ScribeLogHandler();

    void AddLog(const std::string &category, const std::string &buff);

    void AddErrorLog(const std::string &category, const std::string &buff);

	int WriteLog();

	static int service_port;
    static std::string service_address;

private:
    std::vector<LogEntry> messages;
    std::vector<LogEntry> err_messages;

};

} // namespace sclog
} // namespace lspf


#endif //#ifndef LSPF_SCLOG_HANDLER_H
