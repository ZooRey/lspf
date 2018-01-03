#include "sync_comm.h"
#include "async_mq_client.h"
#include "log/log.h"
#include "common/app_conf.h"
#include "common/string_utility.h"
#include <boost/make_shared.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <vector>
#include <string>

SyncComm* SyncComm::instance_ = NULL;

SyncComm* SyncComm::Instance()
{
    if (instance_ == NULL)
	{
        instance_ =  new SyncComm();
    }
    return instance_;
}

void SyncComm::Init(const int m_timeout)
{
	m_session_cache.SetTimeout(m_timeout);
}

void SyncComm::Run()
{
    std::vector<std::string> timeout_key_list;
    std::vector<FutureSessionPtr> timeout_session_list;

    while(1){
        m_session_cache.ClearTimeout(timeout_session_list, timeout_key_list);

        for (std::size_t i=0; i<timeout_key_list.size(); ++i){
            timeout_session_list[i]->SetValue("");
            PLOG_ERROR("Future Session get value failed, id=%s", timeout_key_list[i].c_str());
        }

        timeout_key_list.clear();
        timeout_session_list.clear();

        sleep(1);
    }
}


bool SyncComm::SendAndRecv(const std::string &id, const std::string &request, std::string &response)
{
    FutureSessionPtr session(new FutureSession);

	PLOG_INFO("session cache push, id=%s", id.c_str());
    if (!m_session_cache.PushSession(id, session)){
        PLOG_ERROR("error: PushSession failed, id=%s", id.c_str());
        return false;
    }

    AsyncMQClient::Instance()->SendToQueue(request);

    PLOG_INFO("wait for response, id=%s", id.c_str());
    response = session->GetValue();
    PLOG_INFO("get response success, id=%s, response=%s", id.c_str(), response.c_str());
	if (response.empty())
	{
		return false;
	}
    return true;
}

bool SyncComm::Return(const std::string &id, std::string &response)
{
    FutureSessionPtr session;
	PLOG_INFO("session cache pop, id=%s", id.c_str());
    if (!m_session_cache.PopSession(id, &session)){
        PLOG_ERROR("error: PopSession failed, id=%s", id.c_str());
        return false;
    }

    if (session == NULL){
        PLOG_ERROR("error: PopSession failed, session is null, id=%s", id.c_str());
        return false;
    }
	PLOG_DEBUG("ssession cache set value, id=%s, data=%s", id.c_str(), response.c_str());
    session->SetValue(response);
    PLOG_INFO("send response to promise");

    return true;
}
