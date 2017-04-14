#ifndef LSPF_SCLOG_HANDLER_H
#define LSPF_SCLOG_HANDLER_H

#include "gen-cpp/scribe.h"
#include "common/blocking_queue.h"
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

using namespace scribe::thrift;

namespace lspf {
namespace log {

//�ݽ�֧���̳߳��ռ���־����֧�ֲ����½������̼߳�¼��־
class ScribeLogHandler {
public:
    static void WriteLog();

    static void SetLogCategory(const std::string &category);

    static void SetErrorCategory(const std::string &category);

	static int service_port;
    static std::string service_address;

private:
    static int WriteLogHandle(scribeClient *client);

private:
    ScribeLogHandler(){}
    ~ScribeLogHandler(){}

    static std::vector<LogEntry> messages;
    static std::vector<LogEntry> err_messages;

    static BlockingQueue<LogEntry> msg_queue;
    static BlockingQueue<LogEntry> err_msg_queue;
};

} // namespace sclog
} // namespace lspf


#endif //#ifndef LSPF_SCLOG_HANDLER_H
