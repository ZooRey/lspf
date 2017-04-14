#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <boost/thread/thread.hpp>

#include "common/string_utility.h"
#include "log/log.h"
#include "log/scribe_log.h"

using namespace lspf::log;

void thread_send(int id)
{
	lspf::log::Log::SetLogId(StringUtility::IntToString(id));

	for (int i=0; i<1000000; i++){
		char msg[100] = {0};
		sprintf(msg, "test log num = %d", i);
		PLOG_INFO(msg);
		usleep(1);
	}
}

int main(){
	int rc;

	TLogFunc tlogfunc = boost::bind(&lspf::log::ScribeLog::Write, _1, _2, _3, _4, _5, _6);

	lspf::log::Log::RegisterTlogFunc(tlogfunc, NULL);

	ScribeLog::SetHostAndPort("172.20.11.100", 1463);

	lspf::log::Log::SetLogPriority("INFO");

	ScribeLog::Start();

	sleep(5);


    boost::thread_group group;

    for(int num=1;num<10;num++)
        group.create_thread(boost::bind(&thread_send, num));
    group.join_all();

    std::cout << "finish" << std::endl;


	//while( std::cin.get() != 'q') {}
	return 0;
}
