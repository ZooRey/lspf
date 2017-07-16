#ifndef FUTURE_SESSION_H
#define FUTURE_SESSION_H

#include "session_cache.h"

#include <set>
#include <list>
#include <string>
#include <ctime>
#include <boost/thread/future.hpp>

using namespace std;

#include "common/mutex.h"
#include "log/log.h"

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

template<>
class SessionCache<FutureSessionPtr, std::string>
{
public:
    SessionCache(int timeout_ = 0):m_timeout(timeout_)
    {
        ClearAll();
    }

    ~SessionCache()
    {
        ClearAll();
    }

    void SetTimeout(const int timeout_)
    {
        std::cout << "set time out to " << timeout_ << std::endl;
        m_timeout = timeout_;
    }

    void ClearAll()
    {
        AutoLocker lock(&m_mutex);
        m_list.clear();
        m_map.clear();
    }

    size_t Size()
    {
        AutoLocker lock(&m_mutex);
        assert(m_list.size() == m_map.size());
        return m_map.size();
    }

    //插入节点
    bool PushSession(const std::string& key_, const FutureSessionPtr& value_)
    {
        AutoLocker lock(&m_mutex);
        map_It mapIt = m_map.find(key_);
        if(mapIt == m_map.end())
        {
            Node node;
            node.value = value_;
            node.key = key_;
            node.timestamp = time(NULL);

            m_list.push_back(node);
            list_It listIt = --m_list.end();
            m_map.insert(std::pair<std::string, list_It>(key_, listIt));
            //std::cout << "Push value, key:value is  " << key_ << " : " << value_ << std::endl;
            return true;
        }
        else
        {
            //std::cout << "Push value failed, key is  " << key_ << std::endl;
            return false;
        }
    }

    //查找结点
    bool FindSession(const std::string& key_, FutureSessionPtr& value_)
    {
        AutoLocker lock(&m_mutex);
        map_It mapIt = m_map.find(key_);
        if(mapIt == m_map.end())
        {
            //std::cout << "find session failed, key is   " << key_ << std::endl;
            return false;
        }
        value_ = mapIt->second->value;
        assert(mapIt->second->key == key_);
        //std::cout << "find session OK, key:value is  " << key_ << " : " << value_ << std::endl;
        return true;
    }

    //删除指定key的结点，并返回结点值
    bool PopSession(const std::string& key_, FutureSessionPtr *value_)
    {
        AutoLocker lock(&m_mutex);
        map_It mapIt = m_map.find(key_);
        if(mapIt == m_map.end())
        {
            //std::cout << "Pop session failed, key is   " << key_ << std::endl;
            return false;
        }
        *value_ = mapIt->second->value;
        assert(mapIt->second->key == key_);
        m_list.erase(mapIt->second);
        m_map.erase(key_);
        //std::cout << "Pop session OK, key:value   " << key_ << " : " << value_ << std::endl;
        return true;
    }

    //删除指定key的结点
    bool DelSession(const std::string& key_)
    {
        AutoLocker lock(&m_mutex);
        map_It mapIt = m_map.find(key_);
        if(mapIt != m_map.end())
        {
            //std::cout << "Del session ok, key:value is   " << key_ << " : " << mapIt->second->value << std::endl;
            m_list.erase(mapIt->second);
            m_map.erase(key_);
            return true;
        }
        else
        {
            //std::cout << "Del session failed, key is  " << key_ << std::endl;
            return false;
        }
    }

    //删除并返回首结点
    bool PopFirstSession(std::string& key_, FutureSessionPtr& value_)
    {

        AutoLocker lock(&m_mutex);
        if(Size() == 0)
        {
            //std::cout << "Pop first session get empyt" << std::endl;
            return false;
        }
        list_It listIt = m_list.begin();
        key_ = listIt->key;
        value_ = listIt->value;
        m_map.erase(key_);
        m_list.erase(listIt);
        //std::cout << "Pop first session OK, key:value is  " << key_ << " : " << value_ << std::endl;
        return true;
    }

    //删除超时的结点
    void ClearTimeout()
    {
        if(m_timeout == 0)
        {
            return;
        }
        AutoLocker lock(&m_mutex);
        list_It listIt = m_list.begin();
        int curtime = time(NULL);

        std::string key;
        while(listIt != m_list.end()){
            if(curtime - node.timestamp < m_timeout){
                ///没有节点超时
                break;
            }

            key = listIt->key;
            ///设置值返回
            listIt->value.SetValue("");

            list_It listIt1 = listIt;
            listIt++;
            m_list.erase(listIt1);
            m_map.erase(key);
        }
    }

    //根据指定超时时长删除结点
    void ClearTimeout(const int timeout)
    {
        AutoLocker lock(&m_mutex);
        list_It listIt = m_list.begin();
        int curtime = time(NULL);

        std::string key;
        while(listIt != m_list.end()){
            if(curtime - node.timestamp < m_timeout){
                ///没有节点超时
                break;
            }

            key = listIt->key;
            ///设置值返回
            listIt->value.SetValue("");

            list_It listIt1 = listIt;
            listIt++;
            m_list.erase(listIt1);
            m_map.erase(key);
        }
    }

    //删除超时的结点，并返回被删除的节点
    void ClearTimeout(std::vector<FutureSessionPtr>& valuevec, std::vector<std::string>& keyvec)
    {
        if(m_timeout == 0)
        {
            return;
        }
        AutoLocker lock(&m_mutex);
        list_It listIt = m_list.begin();
        int curtime = time(NULL);
        Node node;
        while(listIt != m_list.end())
        {
            node = *listIt;
            if(curtime - node.timestamp >= m_timeout)
            {
                //std::cout << "remove timeout node ok, key:value is  " << node.key << " : " << node.value << std::endl;
                list_It listIt1 = listIt;
                listIt++;
                valuevec.push_back(node.value);
                keyvec.push_back(node.key);
                m_list.erase(listIt1);
                m_map.erase(node.key);
            }
            else
            {
                break;
            }
        }
    }

private:
    typedef struct
    {
        FutureSessionPtr value;
        std::string key;
        int timestamp;
    }Node;

    typedef  std::list<Node>::iterator list_It;
    typedef  std::map<std::string, list_It>::iterator map_It;

private:
    TMutex m_mutex;
    int m_timeout;
    std::map<std::string, list_It> m_map;
    std::list<Node> m_list;
};
//int SessionCache<CommSessionPtr>::session_timeout = 50;
//TMutex SessionCache<CommSessionPtr>::mutex;

#endif
