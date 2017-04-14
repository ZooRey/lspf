#include "trade_server.h"
#include "common/string_utility.h"
#include "log/log.h"
#include <json/json.h>
#include <boost/scoped_ptr.hpp>
#include <iostream>
#include <stdio.h>
#include <assert.h>

void CupsChannelHandler::DoTrans(std::string& _return, const std::string& logid, const std::string& request)
{
	
	std::cout << request << std::endl;
	_return = "ret:" + request;
}


