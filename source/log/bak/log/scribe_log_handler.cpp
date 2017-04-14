#include "scribe_log_handler.h"

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

std::string ScribeLogHandler::service_address = "";
int ScribeLogHandler::service_port = 0;

ScribeLogHandler::~ScribeLogHandler()
{
	if (!messages.empty() || !err_messages.empty()){
		WriteLog();
	}
}

void ScribeLogHandler::AddLog(const std::string &category, const std::string &buff)
{
    LogEntry LogEntry_;
    LogEntry_.category.assign(category);
    LogEntry_.message.assign(buff);
    messages.push_back(LogEntry_);
}

void ScribeLogHandler::AddErrorLog(const std::string &category, const std::string &buff)
{
    LogEntry errLogEntry_;
    errLogEntry_.category.assign(category);
    errLogEntry_.message.assign(buff);
    err_messages.push_back(errLogEntry_);
}

int ScribeLogHandler::WriteLog()
{
    if (messages.empty() && err_messages.empty())
    {
        return 0;
    }

    try{
        int ret;
        boost::shared_ptr<TTransport> socket(new TSocket(service_address, service_port));
        boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        boost::shared_ptr<scribeClient>  client(new scribeClient(protocol));

        transport->open();

        if (!messages.empty()){
            ret = client->Log(messages);
            if (ret == ResultCode::TRY_LATER){
				//PLOG_FATAL("WriteLog Error");
                return -1;
            }
            messages.clear();
        }

        if (!err_messages.empty()){
            ret = client->Log(err_messages);
            if (ret == ResultCode::TRY_LATER){
				//PLOG_FATAL("WriteLog Error");
                return -1;
            }
            err_messages.clear();
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
