#include "route_dispath.h"
#include <memory>
#include <json/json.h>
#include "common/app_conf.h"
#include "log/log.h"
#include "mexception.h"
#include "gen-channel/PaymentChannel.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace payment_channel;

RouteDispath::RouteDispath(const std::string &channel_id)
    :m_channel_id(channel_id)
{

}

RouteDispath::~RouteDispath()
{

}


int RouteDispath::Query(const std::string &request, std::string &response)
{
    PLOG_INFO("send to channel:%s", request.c_str());

    AppConf *conf = AppConf::Instance();

    std::string ip = conf->GetConfStr("CHN_" + m_channel_id, "ServiceIp");
    int port = conf->GetConfInt("CHN_" + m_channel_id, "ServicePort");

    try {
        boost::shared_ptr<TSocket> socket(new TSocket(ip, port));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        PaymentChannelClient client(protocol);

    	socket->setConnTimeout(5 * 1000);
        socket->setSendTimeout(10 * 1000);
        socket->setRecvTimeout(50 * 1000);

        transport->open();

        client.query(response, lspf::log::Log::GetLogId(), request);

    } catch (TTransportException& tex) {
        PLOG_FATAL("TTransportException:%s", tex.what());
        if (tex.getType() == TTransportException::NOT_OPEN){
            return 1;
        }
        return -1;
    } catch (TException& tx) {
        PLOG_FATAL("TException:%s", tx.what());
        return -1;
    }

    return 0;
}

int RouteDispath::Pay(const std::string &request, std::string &response)
{
    PLOG_INFO("send to channel:%s", request.c_str());

    AppConf *conf = AppConf::Instance();

    std::string ip = conf->GetConfStr("CHN_" + m_channel_id, "ServiceIp");
    int port = conf->GetConfInt("CHN_" + m_channel_id, "ServicePort");

    PLOG_DEBUG("Pay >>>>>ip:%s, port:%d, m_channel_id:%s ", ip.c_str(), port,m_channel_id.c_str());


    try {
        boost::shared_ptr<TSocket> socket(new TSocket(ip, port));
        boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
        boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
        PaymentChannelClient client(protocol);

    	socket->setConnTimeout(5 * 1000);
        socket->setSendTimeout(10 * 1000);
        socket->setRecvTimeout(50 * 1000);

        transport->open();

        client.pay(response, lspf::log::Log::GetLogId(), request);

    } catch (TTransportException& tex) {
        PLOG_FATAL("TTransportException:%s, ip=[%s], port=[%d]", tex.what(), ip.c_str(), port);
        if (tex.getType() == TTransportException::NOT_OPEN){
            return 1;
        }

        return -1;
    } catch (TException& tx) {
        PLOG_FATAL("TException:%s, ip=[%s], port=[%d]", tx.what(), ip.c_str(), port);
        return -1;
    }

    return 0;
}
