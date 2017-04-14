
#include "condition_variable.h"

#include <assert.h>
#include <sys/time.h>
#include <stdexcept>
#include <stdint.h>
#include <string>

ConditionVariable::ConditionVariable() {
    pthread_condattr_t cond_attr;
    pthread_condattr_init(&cond_attr);
    pthread_cond_init(&m_hCondition, &cond_attr);
    pthread_condattr_destroy(&cond_attr);
}

ConditionVariable::~ConditionVariable() {
    pthread_cond_destroy(&m_hCondition);
}

void ConditionVariable::CheckValid() const
{
    assert(m_hCondition.__data.__total_seq != -1ULL && "this cond has been destructed");
}

void ConditionVariable::Signal() {
    CheckValid();
    pthread_cond_signal(&m_hCondition);
}

void ConditionVariable::Broadcast() {
    CheckValid();
    pthread_cond_broadcast(&m_hCondition);
}

void ConditionVariable::Wait(TMutex* mutex) {
    CheckValid();
    pthread_cond_wait(&m_hCondition, mutex->GetMutex());
}

int ConditionVariable::TimedWait(TMutex* mutex, int timeout_in_ms) {
    // -1 转为无限等
    if (timeout_in_ms < 0) {
        Wait(mutex);
        return 0;
    }

    timespec ts;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t usec = tv.tv_usec + timeout_in_ms * 1000LL;
    ts.tv_sec = tv.tv_sec + usec / 1000000;
    ts.tv_nsec = (usec % 1000000) * 1000;

    return pthread_cond_timedwait(&m_hCondition, mutex->GetMutex(), &ts);
}