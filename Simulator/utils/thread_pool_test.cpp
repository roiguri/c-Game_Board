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
    ThreadPool pool(1); // Single thread to create queue buildup
    std::atomic<int> counter{0};
    std::vector<std::future<void>> futures;
    
    // Submit more tasks than threads to test queueing
    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.enqueue([&counter, i]() {
            if (i == 0) {
                // First task sleeps to allow queue buildup
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            counter++;
        }));
    }
    
    // Queue should be larger than 0 while tasks are pending
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_GT(pool.getQueueSize(), 0);
    
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