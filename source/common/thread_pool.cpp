
#include <iostream>

#include "thread_pool.h"


ThreadPool::ThreadPool() : m_exit(false) {
}


ThreadPool::~ThreadPool() {
    Terminate();
}

int ThreadPool::Init(size_t thread_num /* = 4 */) {
    if (thread_num <= 0) {
        return -1;
    }

    for (size_t i = 0; i < thread_num; i++) {

        InnerThread* thread = new InnerThread(&m_pending_queue, &m_finished_queue);
        m_threads.push_back(thread);

        thread->Start();
    }

    return 0;
}

int ThreadPool::AddTask(boost::function<void()>& fun, int64_t task_id) {
    Task t;
    t.fun = fun;
    t.task_id = task_id;

    m_pending_queue.PushBack(t);
    return 0;
}

void ThreadPool::Terminate(bool waiting /* = true */) {
    m_exit = true;
    for (size_t i = 0; i < m_threads.size(); i++) {
        m_threads[i]->Terminate(waiting);
    }

    for (size_t i = 0; i < m_threads.size(); i++) {
        m_threads[i]->Join();
        delete m_threads[i];
    }

    m_pending_queue.Clear();
    m_finished_queue.Clear();
    m_threads.clear();
}

bool ThreadPool::GetFinishedTaskID(int64_t* task_id) {
    if (m_finished_queue.IsEmpty()) {
        return false;
    }

    bool ret = m_finished_queue.TimedPopFront(task_id, 1);
    return ret;
}

ThreadPool::InnerThread::InnerThread(BlockingQueue<Task>* pending_queue,
    BlockingQueue<int64_t>* finished_queue) :
        m_pending_queue(pending_queue),
        m_finished_queue(finished_queue),
        m_exit(false),
        m_waiting(true) {
}

void ThreadPool::InnerThread::Run() {
    //std::cout << "InnerThread id: " << pthread_self() << std::endl;

    while (1) {

        if (m_exit && ((!m_waiting) || (m_waiting && m_pending_queue->IsEmpty()))) {
            break;
        }

        Task t;
        bool ret = m_pending_queue->TimedPopFront(&t, 1000);
        if (ret) {
            t.fun();
            m_finished_queue->PushBack(t.task_id);
        }
    }
}

void ThreadPool::InnerThread::Terminate(bool waiting /* = true */) {
    m_exit = true;
    m_waiting = waiting;
}

size_t ThreadPool::PendingSize()
{
    return m_pending_queue.Size();
}
