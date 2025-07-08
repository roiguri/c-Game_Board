#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>
#include <type_traits>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads = 1);
    ~ThreadPool();

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    template<typename F, typename... Args>
    auto enqueue(F&& func, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type>;

    void waitForAll();
    void shutdown();
    size_t getNumThreads() const;
    size_t getQueueSize() const;
    bool isSingleThreaded() const;

private:
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_tasks;
    
    mutable std::mutex m_queueMutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_stop{false};
    std::atomic<size_t> m_activeTasks{0};
    std::condition_variable m_finishedCondition;
    std::mutex m_finishedMutex;
    
    void workerThread();
};

template<typename F, typename... Args>
auto ThreadPool::enqueue(F&& func, Args&&... args) 
    -> std::future<typename std::result_of<F(Args...)>::type> {
    
    using ReturnType = typename std::result_of<F(Args...)>::type;
    
    auto task = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(func), std::forward<Args>(args)...)
    );
    
    std::future<ReturnType> result = task->get_future();
    
    // If single-threaded, execute immediately on main thread
    if (isSingleThreaded()) {
        if (m_stop) {
            throw std::runtime_error("Cannot enqueue task on stopped ThreadPool");
        }
        (*task)();
        return result;
    }
    
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        
        if (m_stop) {
            throw std::runtime_error("Cannot enqueue task on stopped ThreadPool");
        }
        
        m_tasks.emplace([task]() {
            (*task)();
        });
        
        m_activeTasks++;
    }
    
    m_condition.notify_one();
    return result;
}