#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>

class ThreadPool {
public:
    ThreadPool()
    {
        const uint32_t num_threads = std::thread::hardware_concurrency() - 20;
        for (uint32_t ii = 0; ii < num_threads; ++ii) {
            threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
        }
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(job_queue_mutex);
            should_terminate = true;
        }
        mutex_condition.notify_all();
        for (std::thread& active_thread : threads) {
            active_thread.join();
        }
        threads.clear();
    }

    void queueJob(const std::function<void()>& job)
    {
        {
            std::unique_lock<std::mutex> lock(job_queue_mutex);
            job_queue.push(job);
        }
        mutex_condition.notify_one();
    }

    bool busy()
    {
        bool busy;
        {
            std::unique_lock<std::mutex> lock(job_queue_mutex);
            busy = !job_queue.empty();
        }
        return busy;

    }

private:
    void ThreadLoop()
    {
        while (true) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(job_queue_mutex);
                mutex_condition.wait(lock, [this] {
                    return !job_queue.empty() || should_terminate;
                });
                if (should_terminate) {
                    return;
                }
                job = job_queue.front();
                job_queue.pop();
            }
            job();
        }

    }

    bool should_terminate = false;
    std::vector<std::thread> threads;
    std::mutex job_queue_mutex;
    std::queue<std::function<void()>> job_queue;
    std::condition_variable mutex_condition;
};
