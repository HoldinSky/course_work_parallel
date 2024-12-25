#ifndef CW_API_SOCKET_UTILS_H
#define CW_API_SOCKET_UTILS_H

#include <cstdint>
#include <chrono>
#include <cstring>
#include <mutex>
#include <set>

#define DEFAULT_PORT 2772
#define BUFFER_SIZE 32768
#define SERVER_ADDR "127.0.0.1"

using lock_guard = std::unique_lock<std::mutex>;

constexpr uint32_t strLength(const char* const str)
{
    uint32_t len = 0;

    for (; str[len] != '\0'; len++)
    {
    }

    return len + 1;
}

struct SocketMessageWrapper
{
    int32_t length;
    char* data;
};

struct Commands
{
    // server -> client commands
    static constexpr const char* const listeningTheSocket = "listening"; // ready to receive data
    static constexpr int32_t listeningTheSocketLen = strLength(listeningTheSocket);

    static constexpr const char* const processDone = "done"; // informing client of the end of resource-consuming task
    static constexpr int32_t processDoneLen = strLength(processDone);

    // client -> server commands
    static constexpr const char* const getProgress = "smp"; // send me progress
    static constexpr int32_t getProgressLen = strLength(getProgress);
};

void printErrorAndHalt(const char* msg);

void ReadFromSocketToWrapper(uint32_t const socket, struct SocketMessageWrapper* const msg, int const flags);

void SendStringList(uint32_t const socket, std::set<std::string> const& stringList);

// c-style string manipulation

void populateMessageWithBytes(
    char* buf,
    uint32_t maxSize,
    uint32_t& currentSize,
    size_t dataSize,
    char* ptrToData
);

/// updates 'currentSize' variable in accordance to count of bytes added to 'buf'.
/// 'currentSize' increases by the size of 'data'
template <typename T>
void appendToMessage(char* buf, uint32_t& currentSize, const uint32_t maxSize, T data)
{
    const auto ptr = reinterpret_cast<char*>(&data);
    const size_t dataSize = sizeof(data);

    populateMessageWithBytes(buf, maxSize, currentSize, dataSize, ptr);
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
void startMessage(char* buf, int32_t& currentSize, uint32_t maxSize, const char* data);

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

void resetTimeout(int32_t s_fd, int32_t timeoutType);

#endif // CW_API_SOCKET_UTILS_H
