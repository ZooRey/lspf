#ifndef COMM_SESSION_H
#define COMM_SESSION_H

#include "session_cache.h"

#include <set>
#include <list>
#include <string>
#include <ctime>
#include <boost/thread/future.hpp>

using namespace std;

#include "common/mutex.h"
#include "log/log.h"

class CommSession
{
public:
    CommSession():m_future(m_promise.get_future()){}
    ~CommSession(){}

    void SetValue(const std::string &value){
		try
		{
			m_promise.set_value(value);
		}
		catch(...)
		{

		}
    }

    std::string GetValue(){
        m_future.wait();
        return m_future.get();
    }

private:
    boost::promise<std::string> m_promise;
    boost::unique_future<std::string> m_future;
};

typedef boost::shared_ptr<CommSession> CommSessionPtr;

template<>
class SessionCache<CommSessionPtr>
{
public:
    SessionCache():session_timeout(30){}

	~SessionCache(){}

    size_t Size()
    {
        AutoLocker locker(&mutex);
        return lst_session_record.size();
    }

    void SetTimeout(const int timeout)
    {
        session_timeout = timeout;
    }

	bool PushSession(const string &key_, const CommSessionPtr &value_)
	{
        AutoLocker locker(&mutex);

        if (set_session.find(key_) != set_session.end()){
            return false;
        }

        time_t timeNow = 0;
        timeNow = time(NULL);
        SessionRecord session_record;
        //记录插入时间
        session_record.session_time = timeNow;
        session_record.key = key_;
        session_record.value = value_;

        set_session.insert(key_);
        lst_session_record.push_back(session_record);

        return true;
	}

    bool PopSession(const string &key_, CommSessionPtr *value_)
    {
        AutoLocker locker(&mutex);

        list< SessionRecord>::iterator it;
        it = lst_session_record.begin();
        for(; it != lst_session_record.end(); ++it){
            if (it->key == key_){
                *value_ = it->value;
                set_session.erase(key_);
                lst_session_record.erase(it);

                return true;
            }
        }
        return false;
    }

	void ClearTiomeout()
	{
	    AutoLocker locker(&mutex);
		time_t timeNow = 0;
		timeNow = time(NULL) ;

        list<SessionRecord>::iterator it;
		list<SessionRecord>::iterator itTimeOut;

		it = lst_session_record.begin();
		itTimeOut = lst_session_record.end();
		//查找超时的数据
		for(; it != lst_session_record.end(); ++it){
			if (difftime(timeNow, it->session_time) < session_timeout){
				break;
			}else{
			    it->value->SetValue("");
	            itTimeOut = it;
	            set_session.erase(it->key);
			}
		}

		//删除超时的数据
	    if (itTimeOut != lst_session_record.end()){
			//erase(first,last):first,last分别指向一个序列中初始及末尾位置的迭代器。
			//这个范围即 [first,last)，包括 first 到 last 间的所有元素，包括 first 指向的元素，但不包括 last 指向的元素。
			lst_session_record.erase(lst_session_record.begin(), ++itTimeOut);
	    }
	}

private:
    struct SessionRecord
    {
        long session_time;
        string key;
        CommSessionPtr value;
    };

private:
    int session_timeout;
	TMutex mutex;
	set<string> set_session;
	list<SessionRecord> lst_session_record;

};

//int SessionCache<CommSessionPtr>::session_timeout = 50;
//TMutex SessionCache<CommSessionPtr>::mutex;

#endif
