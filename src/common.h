#ifndef CW_COMMON_H
#define CW_COMMON_H

#include "cstdint"
#include <chrono>
#include <mutex>

#define DEFAULT_PORT 2772
#define BUFFER_SIZE 32768
#define SERVER_ADDR "127.0.0.1"

using lock_guard = std::unique_lock<std::mutex>;

constexpr uint32_t strLength(const char *const str) {
    uint32_t len = 0;

    for (; str[len] != '\0'; len++) {
    }

    return len + 1;
}

struct Commands {
    static constexpr const char *const getProgress = "smp"; // send me progress
    static constexpr int32_t getProgressLen = strLength(getProgress);

    static constexpr const char *const readyReceiveData = "rtr"; // ready to receive [data]
    static constexpr int32_t readyReceiveDataLen = strLength(readyReceiveData);

    static constexpr const char *const start_task = "stt"; // start the task
    static constexpr int32_t start_task_len = strLength(start_task);

    static constexpr const char *const result_ready = "rar"; // results are ready
    static constexpr int32_t result_ready_len = strLength(result_ready);

    static constexpr const char *const get_results = "smr"; // send me results
    static constexpr int32_t get_results_len = strLength(get_results);

    static constexpr const char *const emergency_exit = "emergency";
    static constexpr int32_t emergency_exit_len = strLength(emergency_exit);

    static constexpr const char *const data_received = "rok"; // received - ok
    static constexpr int32_t data_received_len = strLength(data_received);
};

void printErrorAndHalt(const char *msg);

void populateMessageWithBytes(
    char *buf,
    uint32_t maxSize,
    uint32_t &currentSize,
    size_t dataSize,
    char *ptrToData
);

/// updates 'currentSize' variable in accordance to count of bytes added to 'buf'.
/// 'currentSize' increases by the size of 'data'
template<typename T>
void appendToMessage(char *buf, uint32_t &currentSize, const uint32_t maxSize, T data) {
    const auto ptr = reinterpret_cast<char *>(&data);
    const size_t dataSize = sizeof(data);

    populateMessageWithBytes(buf, maxSize, currentSize, dataSize, ptr);
}

/// rewrites passed 'data' at the beginning of 'buf'.
/// 'currentSize' becomes the size of 'data'
template<typename T>
void startMessage(char *buf, int32_t &currentSize, const uint32_t maxSize, T data) {
    currentSize = 0;
    appendToMessage(buf, currentSize, maxSize, data);
}

/// 'currentSize' becomes the size of 'data' excluding '\0'
void appendToMessage(char *buf, int32_t &currentSize, uint32_t maxSize, const char *data);

/// rewrites passed message 'data' at the beginning of 'buf'.
/// 'currentSize' becomes the size of 'data' excluding '\0'
void startMessage(char *buf, int32_t &currentSize, uint32_t maxSize, const char *data);

bool safeSend(uint32_t socket_fd, const void *buf, int32_t n, int32_t flags);

/// update 'current_position' as it goes through message to get bytes
template<typename T>
T parseMessage(char *msg, int32_t &currentPos) {
    T *data = reinterpret_cast<T *>(msg + currentPos);
    currentPos += sizeof(T);

    return *data;
}

void setTimeout(uint32_t s_fd, int32_t timeoutType, int32_t secs, int32_t usec = 0);

void resetTimeout(int32_t s_fd, int32_t timeoutType);

#endif // CW_COMMON_H
