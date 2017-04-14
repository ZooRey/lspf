#include "busi_server.h"

#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <boost/scoped_ptr.hpp>

#include "log/log.h"
#include "mexception.h"

#include "busi_handler.h"
#include "query_handler.h"
#include "payment_handler.h"

enum BUSI_HANDLER{
    BUSI_QUERY,
    BUSI_PAY
};


void AccPaymentHandler::query(std::string& _return, const std::string& logid, const std::string& request){

    try{
        lspf::log::Log::SetLogId(logid);
        PLOG_INFO("query request:%s", request.c_str());

        boost::scoped_ptr<BusiHandler> handler(getHandler(BUSI_QUERY));

        assert(handler.get() != NULL);

        handler->method(request, _return);

        PLOG_INFO("query return:%s", _return.c_str());

    }catch(MException &mx){
        PLOG_ERROR("exception:%s", mx.what());
    }catch(TException &mx){
        PLOG_ERROR("thrift exception:%s", mx.what());
    }
}

void AccPaymentHandler::pay(std::string& _return, const std::string& logid, const std::string& request){

    try{
        lspf::log::Log::SetLogId(logid);
        PLOG_INFO("pay request:%s", request.c_str());

        boost::scoped_ptr<BusiHandler> handler(getHandler(BUSI_PAY));

        assert(handler.get() != NULL);

        handler->method(request, _return);

        PLOG_INFO("pay return:%s", _return.c_str());

    }catch(MException &mx){
        PLOG_ERROR("exception:%s", mx.what());
    }catch(TException &mx){
        PLOG_ERROR("thrift exception:%s", mx.what());
    }

}

BusiHandler *AccPaymentHandler::getHandler(const int type){

    switch(type){
    case BUSI_QUERY:
        return new QueryHandler();
    case BUSI_PAY:
        return new PaymentHandler();
    default:
        return NULL;
    }
}
