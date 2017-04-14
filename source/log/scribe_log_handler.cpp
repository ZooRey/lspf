#include "scribe_log_handler.h"
#include "async_logging.h"

#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

#include "gen-cpp/scribe.h"

using namespace scribe::thrift;

namespace lspf {
namespace log {


static std::string g_log_category = "log";
static std::string g_error_category = "error";

std::string ScribeLogHandler::service_address = "";
int ScribeLogHandler::service_port = 0;
const unsigned int MAX_LOG_SIZE = 32;

std::vector<LogEntry> ScribeLogHandler::messages;
std::vector<LogEntry> ScribeLogHandler::err_messages;

BlockingQueue<LogEntry> ScribeLogHandler::msg_queue;
BlockingQueue<LogEntry> ScribeLogHandler::err_msg_queue;

void ScribeLogHandler::SetLogCategory(const std::string &category)
{
    if (category.empty() || category.size() > 20) {
        return;
    }

    g_log_category = category;
}

void ScribeLogHandler::SetErrorCategory(const std::string &category)
{
    if (category.empty() || category.size() > 20) {
        return;
    }

    g_error_category = category;
}

void ScribeLogHandler::WriteLog()
{
    boost::shared_ptr<TTransport> socket(new TSocket(service_address, service_port));
    boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    boost::shared_ptr<scribeClient> client(new scribeClient(protocol));

    while(1){
        try{
            transport->open();
            if(transport->isOpen()){
                WriteLogHandle(client.get());
            }else{
                sleep(5);
            }

            transport->close();
        }catch (TException& tx) {
            sleep(5);
            continue;
        }
    }
}

int ScribeLogHandler::WriteLogHandle(scribeClient *client)
{
    try{
        int ret = 0;
        while(true){
                std::string log_buffer;

                AsyncLogging::Instance()->Data(log_buffer);
                if (log_buffer.empty()){
                    continue;
                }

                LogEntry log_entry;
                log_entry.category.assign(g_log_category);
                log_entry.message.assign(log_buffer);

                messages.push_back(log_entry);

                ret = client->Log(messages);
                if (ret == ResultCode::TRY_LATER){
                    usleep(100);
                    continue;
                }else{
                    messages.clear();
                }

        }
    }catch (TException& tx) {
        //cout << "ERROR: " << tx.what() << endl;
        //PLOG_FATAL(tx.what());
        return -1;
    }
	return 0;
}

} // namespace sclog
} // namespace lspf
