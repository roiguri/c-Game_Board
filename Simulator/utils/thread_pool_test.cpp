#include <gtest/gtest.h>
#include "thread_pool.h"
#include <chrono>
#include <atomic>
#include <vector>
#include <thread>

class ThreadPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup before each test
    }
    
    void TearDown() override {
        // Cleanup after each test
    }
};

TEST_F(ThreadPoolTest, ConstructorWithDefaultThreads) {
    ThreadPool pool;
    EXPECT_GT(pool.getNumThreads(), 0);
    EXPECT_LE(pool.getNumThreads(), std::thread::hardware_concurrency());
}

TEST_F(ThreadPoolTest, ConstructorWithSpecificThreads) {
    ThreadPool pool(4);
    EXPECT_EQ(pool.getNumThreads(), 4);
}

TEST_F(ThreadPoolTest, SingleThreadedMode) {
    ThreadPool pool(1);
    EXPECT_EQ(pool.getNumThreads(), 0);  // No worker threads
    EXPECT_TRUE(pool.isSingleThreaded());
}

TEST_F(ThreadPoolTest, MultiThreadedMode) {
    ThreadPool pool(3);
    EXPECT_EQ(pool.getNumThreads(), 3);  // 3 worker threads
    EXPECT_FALSE(pool.isSingleThreaded());
}

TEST_F(ThreadPoolTest, ThreadCountNeverEquals2) {
    // Test various thread counts to ensure total threads never equals 2
    
    // numThreads = 1 should give 1 total thread (main only)
    ThreadPool pool1(1);
    EXPECT_EQ(pool1.getNumThreads(), 0);  // 0 workers + 1 main = 1 total
    
    // numThreads = 2 should give 2 total threads (2 workers + 1 main = 3 total)
    // But our implementation creates 2 workers, so total = 3
    ThreadPool pool2(2);
    EXPECT_EQ(pool2.getNumThreads(), 2);  // 2 workers + 1 main = 3 total
    
    // numThreads = 3 should give 3 workers + 1 main = 4 total
    ThreadPool pool3(3);
    EXPECT_EQ(pool3.getNumThreads(), 3);  // 3 workers + 1 main = 4 total
    
    // Verify we never get exactly 2 total threads
    // (1 worker + 1 main = 2 total is forbidden)
}

TEST_F(ThreadPoolTest, ConstructorWithZeroThreads) {
    ThreadPool pool(0);
    EXPECT_GT(pool.getNumThreads(), 0);
}

TEST_F(ThreadPoolTest, SimpleTaskExecution) {
    ThreadPool pool(2);
    std::atomic<int> counter{0};
    
    auto future = pool.enqueue([&counter]() {
        counter++;
        return 42;
    });
    
    int result = future.get();
    EXPECT_EQ(result, 42);
    EXPECT_EQ(counter.load(), 1);
}

TEST_F(ThreadPoolTest, SingleThreadedExecution) {
    ThreadPool pool(1);
    std::atomic<int> counter{0};
    
    // Tasks should execute immediately on main thread
    auto future = pool.enqueue([&counter]() {
        counter++;
        return 42;
    });
    
    int result = future.get();
    EXPECT_EQ(result, 42);
    EXPECT_EQ(counter.load(), 1);
    
    // Multiple tasks should still work
    std::vector<std::future<int>> futures;
    for (int i = 0; i < 5; ++i) {
        futures.push_back(pool.enqueue([&counter, i]() {
            counter++;
            return i * 10;
        }));
    }
    
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(futures[i].get(), i * 10);
    }
    
    EXPECT_EQ(counter.load(), 6);  // 1 + 5 additional tasks
}

TEST_F(ThreadPoolTest, MultipleTaskExecution) {
    ThreadPool pool(4);
    std::atomic<int> counter{0};
    std::vector<std::future<void>> futures;
    
    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.enqueue([&counter]() {
            counter++;
        }));
    }
    
    for (auto& future : futures) {
        future.get();
    }
    
    EXPECT_EQ(counter.load(), 10);
}

TEST_F(ThreadPoolTest, TaskWithParameters) {
    ThreadPool pool(2);
    
    auto future = pool.enqueue([](int a, int b) {
        return a + b;
    }, 5, 3);
    
    int result = future.get();
    EXPECT_EQ(result, 8);
}

TEST_F(ThreadPoolTest, TasksExecuteInParallel) {
    ThreadPool pool(2);
    std::atomic<int> running_tasks{0};
    std::atomic<int> max_concurrent{0};
    
    std::vector<std::future<void>> futures;
    
    for (int i = 0; i < 4; ++i) {
        futures.push_back(pool.enqueue([&running_tasks, &max_concurrent]() {
            int current = ++running_tasks;
            int expected = max_concurrent.load();
            while (expected < current && !max_concurrent.compare_exchange_weak(expected, current)) {
                expected = max_concurrent.load();
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            --running_tasks;
        }));
    }
    
    for (auto& future : futures) {
        future.get();
    }
    
    EXPECT_GT(max_concurrent.load(), 1);
}

TEST_F(ThreadPoolTest, WaitForAllTasks) {
    ThreadPool pool(2);
    std::atomic<int> counter{0};
    
    for (int i = 0; i < 5; ++i) {
        pool.enqueue([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            counter++;
        });
    }
    
    pool.waitForAll();
    EXPECT_EQ(counter.load(), 5);
}

TEST_F(ThreadPoolTest, ExceptionHandling) {
    ThreadPool pool(2);
    
    auto future = pool.enqueue([]() -> int {
        throw std::runtime_error("Test exception");
        return 42;
    });
    
    EXPECT_THROW(future.get(), std::runtime_error);
}

TEST_F(ThreadPoolTest, TaskQueueGrowth) {
    ThreadPool pool(2);
    std::atomic<int> counter{0};
    std::vector<std::future<void>> futures;
    
    // Submit more tasks than threads to test queueing
    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.enqueue([&counter, i]() {
            if (i < 2) {
                // First two tasks sleep to allow queue buildup
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            counter++;
        }));
    }
    
    // With 2 worker threads busy and 10 total tasks, expect 8 tasks queued
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_EQ(pool.getQueueSize(), 8);
    
    for (auto& future : futures) {
        future.get();
    }
    
    EXPECT_EQ(counter.load(), 10);
    EXPECT_EQ(pool.getQueueSize(), 0);
}

TEST_F(ThreadPoolTest, ThreadSafety) {
    ThreadPool pool(4);
    std::atomic<int> shared_counter{0};
    std::vector<std::future<void>> futures;
    
    // Multiple threads incrementing shared counter
    for (int i = 0; i < 100; ++i) {
        futures.push_back(pool.enqueue([&shared_counter]() {
            for (int j = 0; j < 100; ++j) {
                shared_counter++;
            }
        }));
    }
    
    for (auto& future : futures) {
        future.get();
    }
    
    EXPECT_EQ(shared_counter.load(), 10000);
}

TEST_F(ThreadPoolTest, ShutdownPreventsNewTasks) {
    ThreadPool pool(2);
    
    pool.shutdown();
    
    EXPECT_THROW(pool.enqueue([]() { return 42; }), std::runtime_error);
}

TEST_F(ThreadPoolTest, DestructorWaitsForTasks) {
    std::atomic<bool> task_completed{false};
    
    {
        ThreadPool pool(1);
        pool.enqueue([&task_completed]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            task_completed = true;
        });
        // ThreadPool destructor should wait for task completion
    }
    
    EXPECT_TRUE(task_completed.load());
}

TEST_F(ThreadPoolTest, LargeNumberOfTasks) {
    ThreadPool pool(4);
    std::atomic<int> counter{0};
    std::vector<std::future<void>> futures;
    
    const int num_tasks = 1000;
    for (int i = 0; i < num_tasks; ++i) {
        futures.push_back(pool.enqueue([&counter]() {
            counter++;
        }));
    }
    
    for (auto& future : futures) {
        future.get();
    }
    
    EXPECT_EQ(counter.load(), num_tasks);
}

TEST_F(ThreadPoolTest, TaskReturnValues) {
    ThreadPool pool(2);
    
    auto future1 = pool.enqueue([]() { return std::string("Hello"); });
    auto future2 = pool.enqueue([]() { return 3.14; });
    auto future3 = pool.enqueue([]() { return std::vector<int>{1, 2, 3}; });
    
    EXPECT_EQ(future1.get(), "Hello");
    EXPECT_DOUBLE_EQ(future2.get(), 3.14);
    std::vector<int> expected{1, 2, 3};
    EXPECT_EQ(future3.get(), expected);
}

TEST_F(ThreadPoolTest, StressTest) {
    ThreadPool pool(8);
    std::atomic<long> total{0};
    std::vector<std::future<void>> futures;
    
    const int num_tasks = 1000;
    const int work_per_task = 100;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_tasks; ++i) {
        futures.push_back(pool.enqueue([&total, work_per_task]() {
            long local_sum = 0;
            for (int j = 0; j < work_per_task; ++j) {
                local_sum += j;
            }
            total += local_sum;
        }));
    }
    
    for (auto& future : futures) {
        future.get();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    long expected_total = static_cast<long>(num_tasks) * (work_per_task * (work_per_task - 1)) / 2;
    EXPECT_EQ(total.load(), expected_total);
    
    // Test should complete in reasonable time (adjust threshold as needed)
    EXPECT_LT(duration.count(), 5000); // Less than 5 seconds
}