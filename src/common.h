#ifndef CW_COMMON_H
#define CW_COMMON_H

#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include <cstdint>
#include <chrono>

using rwLock = std::shared_mutex;
using sharedLock = std::shared_lock<rwLock>;
using exclusiveLock = std::unique_lock<rwLock>;

static rwLock stdoutLock;
static rwLock commonMutex;

static std::condition_variable_any commonMonitor;

struct Terminal {
    const char *const red = "\033[0;31m";
    const char *const green = "\033[0;32m";
    const char *const yellow = "\033[0;33m";
    const char *const blue = "\033[0;34m";
    const char *const magenta = "\033[0;35m";
    const char *const cyan = "\033[0;36m";
    const char *const white = "\033[0;37m";
    const char *const reset = "\033[0m";
};

static constexpr Terminal terminal{};

template<typename FT>
std::chrono::duration<int64_t, std::milli>
measureExecutionTime(FT func) {
    const auto start = std::chrono::high_resolution_clock::now();
    func();
    const auto end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

/// both indices are inclusive
struct Range
{
    int64_t start{-1};
    int64_t end{-1};

    [[nodiscard]] int64_t length() const
    {
        return end - start + 1;
    }
};

#endif // CW_COMMON_H