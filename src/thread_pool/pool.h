#ifndef CW_THREAD_POOL_POOL_H
#define CW_THREAD_POOL_POOL_H

#include <functional>
#include <thread>
#include <queue>
#include <condition_variable>

#include "common.h"

struct ThreadTask {
    std::function<void()> action;
    uint64_t id{};  // for debug only

    void operator()() const {
        action();
    }

    ThreadTask() = default;
};

class ThreadPool {
public:
    inline explicit ThreadPool(const uint32_t &threadsCount = std::thread::hardware_concurrency())
        : threadsCount(threadsCount) {
        this->workers = std::make_unique<std::thread[]>(this->threadsCount);

        this->initialize();
    }

    inline ~ThreadPool() { terminate(); }

public:
    bool alive() const;

    bool alive_unsafe() const;

    uint32_t currentlyScheduledTasks() const {
        sharedLock _(this->commonLock);
        return this->taskQueue.size();
    }

public:
    void scheduleTask(const ThreadTask& task);

    void terminate();

    void joinAll();

public:
    ThreadPool(ThreadPool const &other) = delete;

    ThreadPool &operator=(ThreadPool const &rhs) = delete;

private:
    std::atomic_bool initialized{false};
    std::atomic_bool terminated{false};

private:
    uint32_t threadsCount;
    std::unique_ptr<std::thread[]> workers;
    std::queue<ThreadTask> taskQueue{};

private:
    mutable rwLock commonLock;
    mutable rwLock joinLock;

private:
    std::condition_variable_any taskWaiter{};
    std::condition_variable_any joinWaiter{};

private:
    void initialize();

    void threadRoutine();

    bool getTaskFromQueue(ThreadTask &outTask);
};

#endif // CW_THREAD_POOL_POOL_H
