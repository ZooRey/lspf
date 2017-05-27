#ifndef SYNC_COMM_H
#define SYNC_COMM_H

#include "common/thread.h"
#include "comm_session.h"
#include <boost/shared_ptr.hpp>
#include <string>

class SyncComm : public Thread
{
public:
	static SyncComm* Instance();
	virtual~SyncComm(){}

public:
	void Init(const int timeout);

    virtual void Run();

    bool SendAndRecv(const std::string &id, const std::string &request, std::string &response);

    bool Return(const std::string &id, std::string &response);

private:
	SyncComm():m_timeout(30){}

private:
    int m_timeout;
	SessionCache<CommSessionPtr> m_session_cache;

	static SyncComm* instance_;
};

#endif //__COMM_MANAGE_H
