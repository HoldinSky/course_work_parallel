#include "pool.h"

bool ThreadPool::alive() const
{
    sharedLock _(this->commonLock);
    return this->alive_unsafe();
}

bool ThreadPool::alive_unsafe() const
{
    return this->initialized && !this->terminated.load();
}

void ThreadPool::initialize()
{
    exclusiveLock _(this->commonLock);
    if (this->initialized || this->terminated.load())
        return;

    for (size_t i = 0; i < this->threadsCount; i++)
        this->workers[i] = std::thread(&ThreadPool::threadRoutine, this);

    this->initialized.store(true);
    this->terminated.store(false);
}

void ThreadPool::joinAll()
{
    exclusiveLock _(this->joinLock);

    this->joinWaiter.wait(_, [&] { return this->taskQueue.empty(); });
}

void ThreadPool::terminate()
{
    {
        exclusiveLock _(this->commonLock);
        if (!alive_unsafe())
            return;

        this->initialized.store(false);
        this->terminated.store(true);
        while(!this->taskQueue.empty())
        {
            this->taskQueue.pop();
        }
        {
            exclusiveLock jL(this->joinLock);
            this->joinWaiter.notify_all();
        }
        this->taskWaiter.notify_all();
    }

    for (size_t i = 0; i < this->threadsCount; i++)
        this->workers[i].join();
}

bool ThreadPool::getTaskFromQueue(ThreadTask& outTask)
{
    exclusiveLock _(this->commonLock);
    bool taskAcquired = false;

    auto continueCondition = [&]
    {
        taskAcquired = !this->taskQueue.empty();
        if (taskAcquired)
        {
            outTask = this->taskQueue.front();
            this->taskQueue.pop();
        }

        return this->terminated || taskAcquired;
    };

    this->taskWaiter.wait(_, continueCondition);

    {
        exclusiveLock jL(this->joinLock);
        this->joinWaiter.notify_all();
    }

    return taskAcquired;
}

void ThreadPool::threadRoutine()
{
    while (true)
    {
        ThreadTask task;

        const bool taskAcquired = this->getTaskFromQueue(task);
        if (this->terminated)
        {
            return;
        }
        if (!taskAcquired)
        {
            continue;
        }

        task.operator()();
        printf("[INFO | ThreadPool] Task %lld is done\n", task.id);
    }
}

void ThreadPool::scheduleTask(const ThreadTask& task)
{
    if (!alive())
        return;

    exclusiveLock _(this->commonLock);
    this->taskQueue.push(task);
    this->taskWaiter.notify_one();
    printf("[INFO | ThreadPool] Task %lld is scheduled\n", task.id);
}
