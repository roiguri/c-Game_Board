#include "thread_pool.h"
#include <algorithm>

ThreadPool::ThreadPool(size_t numThreads) : m_stop(false) {
    if (numThreads == 0) {
        numThreads = 1;
    }
    
    // If numThreads is 1, use single-threaded execution (main thread only)
    // Otherwise, create numThreads worker threads (total = numThreads + main thread)
    if (numThreads == 1) {
        // Single-threaded mode: no worker threads, execute on main thread
        return;
    }
    
    m_workers.reserve(numThreads);
    
    for (size_t i = 0; i < numThreads; ++i) {
        m_workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_stop = true;
    }
    
    m_condition.notify_all();
    
    for (std::thread& worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    m_workers.clear();
}

void ThreadPool::waitForAll() {
    // In single-threaded mode, all tasks are executed immediately
    if (isSingleThreaded()) {
        return;
    }
    
    std::unique_lock<std::mutex> lock(m_finishedMutex);
    m_finishedCondition.wait(lock, [this] {
        return m_activeTasks == 0;
    });
}

size_t ThreadPool::getNumThreads() const {
    return m_workers.size();
}

size_t ThreadPool::getQueueSize() const {
    std::unique_lock<std::mutex> lock(m_queueMutex);
    return m_tasks.size();
}

bool ThreadPool::isSingleThreaded() const {
    return m_workers.empty();
}

void ThreadPool::workerThread() {
    while (true) {
        std::function<void()> task;
        
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            
            m_condition.wait(lock, [this] {
                return m_stop || !m_tasks.empty();
            });
            
            if (m_stop && m_tasks.empty()) {
                return;
            }
            
            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        
        try {
            task();
        } catch (...) {
            // Log error but continue processing
            // In a real implementation, you might want to handle exceptions differently
        }
        
        {
            std::unique_lock<std::mutex> lock(m_finishedMutex);
            m_activeTasks--;
            if (m_activeTasks == 0) {
                m_finishedCondition.notify_all();
            }
        }
    }
}