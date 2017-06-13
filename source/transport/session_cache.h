#ifndef __SESSION_CACHE_H__
#define __SESSION_CACHE_H__

#include <list>
#include <map>
#include <iostream>
#include "string.h"
#include "stdio.h"
#include "assert.h"
#include "common/mutex.h"
#include <vector>

using namespace std;

template<typename value_type, typename key_type = string>
class SessionCache
{
public:
    typedef struct
    {
        value_type value;
        key_type key;
        int timestamp;
    }Node;

    typedef typename std::list<Node>::iterator list_It;
    typedef typename std::map<key_type, list_It>::iterator map_It;
    
    SessionCache(int timeout_ = 0):m_timeout(timeout_)
    {
        clearAll();
    }

    ~SessionCache()
    {
        clearAll();
    }

    void SetTimeout(const int timeout_)
    {
        std::cout << "set time out to " << timeout_ << std::endl;
        m_timeout = timeout_;
    }

    void clearAll()
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
    bool PushSession(const key_type& key_, const value_type& value_)
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
            m_map.insert(std::pair<key_type, list_It>(key_, listIt));
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
    bool FindSession(const key_type& key_, value_type& value_)
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
    bool PopSession(const key_type& key_, value_type *value_)
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
    bool DelSession(const key_type& key_)
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
    bool PopFirstSession(key_type& key_, value_type& value_)
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
        Node node;
        while(listIt != m_list.end())
        {
            node = *listIt;
            if(curtime - node.timestamp >= m_timeout)
            {
                //std::cout << "remove timeout session ok, key:value is  " << node.key << " : " << node.value << std::endl;
                list_It listIt1 = listIt;
                listIt++;
                m_list.erase(listIt1);
                m_map.erase(node.key);
            }
            else
            {
                break;
            }
        }
    }

    //根据指定超时时长删除结点
    void ClearTimeout(const int timeout)
    {
        AutoLocker lock(&m_mutex);
        list_It listIt = m_list.begin();
        int curtime = time(NULL);
        Node node;
        while(listIt != m_list.end())
        {
            node = *listIt;
            if(curtime - node.timestamp >= timeout)
            {
                //std::cout << "remove timeout(timeoutvalue: " << timeout << ") session ok, key:value is  " << node.key << " : " << node.value << std::endl;
                list_It listIt1 = listIt;
                listIt++;
                m_list.erase(listIt1);
                m_map.erase(node.key);
            }
            else
            {
                break;
            }
        }
    }

    //删除超时的结点，并返回被删除的节点
    void ClearTimeout(vector<value_type>& valuevec, vector<key_type>& keyvec)
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
    TMutex m_mutex;
    int m_timeout;
    std::map<key_type, list_It> m_map;
    std::list<Node> m_list;
};

/* Test code
int main()
{
    SessionCache<std::string, int> mylistmap(10);
    int key;
    std::string value;
    char sztmp[10];
    std::cout << "=============1==============="  << std::endl;
    for(int i = 0; i < 5; i ++)
    {
        key = i + 200;
        sprintf(sztmp, "%d", key);
        value = std::string("listmapvalue_") + std::string(sztmp);
        mylistmap.PushSession(key, value);
    }
    std::cout << "=============2==============="  << std::endl;

    for(int i = 0; i < 5; i ++)
    {
        key = i + 200;
        mylistmap.FindSession(key, value);
    }
    std::cout << "=============3==============="  << std::endl;

    for(int i = 0; i < 5; i ++)
    {
        key = i + 200;
        mylistmap.PopSession(key, value);
    }
    std::cout << "=============4==============="  << std::endl;

    for(int i = 0; i < 5; i ++)
    {
        key = i + 200;
        sprintf(sztmp, "%d", key);
        value = std::string("listmapvalue_") + std::string(sztmp);
        mylistmap.PushSession(key, value);
    }
    std::cout << "=============5==============="  << std::endl;

    for(int i = 0; i < 5; i ++)
    {
        key = i + 200;
        mylistmap.DelSession(key);
    }
    std::cout << "=============6==============="  << std::endl;

    for(int i = 0; i < 5; i ++)
    {
        key = i + 200;
        sprintf(sztmp, "%d", key);
        value = std::string("listmapvalue_") + std::string(sztmp);
        mylistmap.PushSession(key, value);
    }
    std::cout << "=============7==============="  << std::endl;

    for(int i = 0; i < 5; i ++)
    {
        key = i + 200;
        mylistmap.PopFirstSession(key, value);
    }
    std::cout << "=============8==============="  << std::endl;

    for(int i = 0; i < 5; i ++)
    {
        key = i + 200;
        sprintf(sztmp, "%d", key);
        value = std::string("listmapvalue_") + std::string(sztmp);
        mylistmap.PushSession(key, value);
    }
    std::cout << "=============9==============="  << std::endl;

    sleep(11);

    mylistmap.ClearTimeout();
    std::cout << "=============10=============="  << std::endl;

    for(int i = 0; i < 5; i ++)
    {
        key = i + 200;
        sprintf(sztmp, "%d", key);
        value = std::string("listmapvalue_") + std::string(sztmp);
        mylistmap.PushSession(key, value);
    }
    std::cout << "=============11==============="  << std::endl;

    sleep(5);
    mylistmap.ClearTimeout(3);
    std::cout << "=============12==============="  << std::endl;

    for(int i = 0; i < 5; i ++)
    {
        key = i + 200;
        sprintf(sztmp, "%d", key);
        value = std::string("listmapvalue_") + std::string(sztmp);
        mylistmap.PushSession(key, value);
    }
    std::cout << "=============13==============="  << std::endl;
    
    sleep(11);
    vector<string> valueVec;
    vector<int> KeyVec;
    mylistmap.ClearTimeout(valueVec, KeyVec);
    for(int i = 0; i < valueVec.size(); i++)
    {
        std::cout << "clear timeout session, key:value " << KeyVec[i] << " : " << valueVec[i]  << std::endl;
    }
    std::cout << "current list size " << mylistmap.Size() << std::endl;

    std::cout << "=============end==============="  << std::endl;
}
*/
#endif