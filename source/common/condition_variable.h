
#ifndef SOURCE_COMMON_CONDITION_VARIABLE_H
#define SOURCE_COMMON_CONDITION_VARIABLE_H

#include <assert.h>
#include <pthread.h>

#include "mutex.h"

class ConditionVariable
{
public:
    ConditionVariable();
    ~ConditionVariable();

    void Signal();

    void Broadcast();

    void Wait(TMutex* mutex);

    // If timeout_in_ms < 0, it means infinite waiting until condition is signaled
    // by another thread
    int TimedWait(TMutex* mutex, int timeout_in_ms = -1);
private:
    void CheckValid() const;
private:
    pthread_cond_t m_hCondition;
};

#endif // SOURCE_COMMON_CONDITION_VARIABLE_H

