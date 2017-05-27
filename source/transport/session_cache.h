#ifndef SESSION_CACHE_H
#define SESSION_CACHE_H

#include <set>
#include <list>
#include <string>
#include <ctime>
#include "common/mutex.h"

using namespace std;


template <typename T>
class SessionCache
{
public:
    SessionCache():session_timeout(30){}

	~SessionCache(){}

    void SetTimeout(const int timeout)
    {
        session_timeout = timeout;
    }

    size_t Size()
    {
        AutoLocker locker(&mutex);
        return lst_session_record.size();
    }

	bool PushSession(const std::string &key_, const T &value_)
	{
        AutoLocker locker(&mutex);

        if (set_session.find(key_) != set_session.end()){
            return false;
        }

        time_t timeNow = 0;
        timeNow = time(NULL);
        SessionRecord session_record;
        //��¼����ʱ��
        session_record.session_time = timeNow;
        session_record.key = key_;
        session_record.value = value_;

        set_session.insert(key_);
        lst_session_record.push_back(session_record);

        return true;
	}

    bool PopSession(const std::string &key_, T *value_)
    {
        AutoLocker locker(&mutex);
        typename std::list<SessionRecord>::iterator it = lst_session_record.begin();
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

        typename std::list<SessionRecord>::iterator it;
		typename std::list<SessionRecord>::iterator itTimeOut;

		it = lst_session_record.begin();
		itTimeOut = lst_session_record.end();
		//���ҳ�ʱ������
		for(; it != lst_session_record.end(); ++it){
			if (difftime(timeNow, it->session_time) < session_timeout){
				break;
			}else{
			    set_session.erase(it->key);
	            itTimeOut = it;
			}
		}

		//ɾ����ʱ������
	    if (itTimeOut != lst_session_record.end()){
			//erase(first,last):first,last�ֱ�ָ��һ�������г�ʼ��ĩβλ�õĵ�������
			//�����Χ�� [first,last)������ first �� last �������Ԫ�أ����� first ָ���Ԫ�أ��������� last ָ���Ԫ�ء�
			lst_session_record.erase(lst_session_record.begin(), ++itTimeOut);
	    }
	}

private:
    struct SessionRecord
    {
        time_t session_time;
        std::string key;
        T value;
    };

private:
    int session_timeout;
	TMutex mutex;
	std::set<std::string> set_session;
	std::list<SessionRecord> lst_session_record;
};


#endif
