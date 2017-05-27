#include "sync_comm.h"
#include "async_mq_client.h"
#include "log/log.h"
#include "common/app_conf.h"
#include "common/string_utility.h"
#include <boost/make_shared.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

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
    while(1){
        m_session_cache.ClearTiomeout();
        sleep(m_timeout);
    }
}


bool SyncComm::SendAndRecv(const std::string &id, const std::string &request, std::string &response)
{
    CommSessionPtr session(new CommSession);

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
    CommSessionPtr session;
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
