#ifndef CW_API_SOCKET_UTILS_H
#define CW_API_SOCKET_UTILS_H

#include <cstdint>
#include <chrono>
#include <cstring>
#include <mutex>
#include <set>

#include "common.h"

#define DEFAULT_PORT 2772
#define SERVER_ADDR "127.0.0.1"

using lock_guard = std::unique_lock<std::mutex>;

void printErrorAndHalt(const char* msg);

/// updates 'currentSize' variable in accordance to count of bytes added to 'buf' = sizeof(data).
uint32_t populateMessageWithBytes(
    char* buf,
    uint32_t& bufSize,
    char const* data,
    size_t dataSize,
    uint32_t maxSize
);

/// updates 'currentSize' variable in accordance to count of bytes added to 'buf'.
/// 'currentSize' increases by the size of 'data'
template <typename T>
void appendToMessage(char* buf, uint32_t& currentSize, const uint32_t maxSize, T data)
{
    const auto ptr = reinterpret_cast<char*>(&data);
    const size_t dataSize = sizeof(data);

    populateMessageWithBytes(buf, currentSize, ptr, dataSize, maxSize);
}

/// rewrites passed 'data' at the beginning of 'buf'.
/// 'currentSize' becomes the size of 'data'
template <typename T>
void startMessage(char* buf, int32_t& currentSize, const uint32_t maxSize, T data)
{
    currentSize = 0;
    appendToMessage(buf, currentSize, maxSize, data);
}

/// 'currentSize' becomes the size of 'data' excluding '\0'
void appendToMessage(char* buf, int32_t& currentSize, uint32_t maxSize, const char* data);

/// rewrites passed message 'data' at the beginning of 'buf'.
/// 'currentSize' becomes the size of 'data' excluding '\0'
void startMessage(char* buf, uint32_t& currentSize, uint32_t maxSize, const char* data);

bool safeSend(uint32_t socket_fd, const void* buf, int32_t n, int32_t flags);

/// update 'current_position' as it goes through message to get bytes
template <typename T>
T parseMessage(char* msg, int32_t& offset)
{
    T* data = reinterpret_cast<T*>(msg + offset);
    offset += sizeof(T);

    return *data;
}

void AppendToString(char** dest, const char* src);

// socket additional params

void setTimeout(uint32_t s_fd, int32_t timeoutType, int32_t secs, int32_t usec = 0);

void resetTimeout(uint32_t s_fd, int32_t timeoutType);

#endif // CW_API_SOCKET_UTILS_H
