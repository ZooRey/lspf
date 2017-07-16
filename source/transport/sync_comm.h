#ifndef SYNC_COMM_H
#define SYNC_COMM_H

#include "common/thread.h"
#include "session_cache.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread/future.hpp>
#include <string>

class FutureSession
{
public:
    FutureSession():is_wait(false), m_future(m_promise.get_future()){}
    ~FutureSession(){
        try{
            if (is_wait){
                m_promise.set_value("");
            }
        }catch(...){

        }
    }

    void SetValue(const std::string &value){
        m_promise.set_value(value);
    }

    std::string GetValue(){
        is_wait = true;
        m_future.wait();
        is_wait = false;
        return m_future.get();
    }

private:
    bool is_wait;
    boost::promise<std::string> m_promise;
    boost::unique_future<std::string> m_future;
};

typedef boost::shared_ptr<FutureSession> FutureSessionPtr;

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
	SessionCache<FutureSessionPtr, std::string> m_session_cache;

	static SyncComm* instance_;
};

#endif //__COMM_MANAGE_H
