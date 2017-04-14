#ifndef COMMON_MESSAGE_POOL_H
#define COMMON_MESSAGE_POOL_H

#include <vector>
#include "mutex.h"

template <typename T>
class MessagePool
{
public:
    MessagePool()
    {
    }

    ~MessagePool()
    {
    }

    void SetMessage(const T &value)
    {
        if (t_index < m_all_message.size()){
            m_all_message[t_index] = value;
            return;
        }

        AutoLocker locker(&m_mutex);
        t_index = m_all_message.size();
        m_all_message.push_back(value);
    }

    void SetMessage(const size_t index, const T &value)
    {
        if (index >= m_all_message.size()){
            return;
        }

        m_all_message[index] = value;
    }

    void GetMessage(T *value)
    {
        if (t_index >= m_all_message.size()){

            return;
        }
        *value = m_all_message[t_index];
    }

    void GetMessage(const size_t index, T *value)
    {
        if (index >= m_all_message.size()){
            return;
        }

        *value = m_all_message[index];
    }

    size_t Size()
    {
        return m_all_message.size();
    }

private:
    TMutex m_mutex;
    std::vector<T> m_all_message;
    static __thread size_t t_index;
};

template <typename T>
__thread size_t MessagePool<T>::t_index = 99999999;



#endif // COMMON_MESSAGE_POOL_H
