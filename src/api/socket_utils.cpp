#include "socket_utils.h"

#include <winsock2.h>
#include <csignal>

void printErrorAndHalt(char const* const msg)
{
    fprintf(stderr, "%s", msg);
    exit(-1);
}

uint32_t populateMessageWithBytes(
    char* buf,
    uint32_t& bufSize,
    char const* const data,
    size_t dataSize,
    uint32_t const maxSize
)
{
    if (bufSize >= maxSize) return 0;
    char const* dataPtr = data;
    uint32_t const startBufSize = bufSize;

    while (dataSize--)
    {
        buf[bufSize] = *dataPtr;
        ++bufSize;
        ++dataPtr;
        if (bufSize == maxSize) break;
    }

    return bufSize - startBufSize;
}

void AppendToString(char** dest, const char* src)
{
    size_t const destLen = *dest ? strlen(*dest) : 0;
    size_t const srcLen = strlen(src);

    char* const newBuffer = static_cast<char*>(realloc(*dest, destLen + srcLen + 1)); // +1 for null terminator
    if (!newBuffer)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    *dest = newBuffer; // Update the pointer
    strcpy(*dest + destLen, src); // Append the source string
}

/// 'current_size' becomes the size of 'data' including '\0'
void appendToMessage(char* buf, uint32_t& current_size, const uint32_t max_size, const char* const data)
{
    const auto ptr = const_cast<char*>(data);
    const size_t data_size = strlen(data);

    populateMessageWithBytes(buf, current_size, ptr, data_size, max_size);
}

/// rewrites passed message 'data' at the beginning of 'buf'.
/// 'current_size' becomes the size of 'data' including '\0'
void startMessage(char* buf, uint32_t& current_size, const uint32_t max_size, const char* const data)
{
    current_size = 0;
    appendToMessage(buf, current_size, max_size, data);
}

/// closes socket in case of error
bool safeSend(const uint32_t socket_fd, const void* buf, const int32_t n, const int32_t flags)
{
    char error = 0;
    int len = sizeof(error);
    int const retval = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);

    char errBuf[256];
    if (retval != 0)
    {
        sprintf(errBuf, "[ERR | SOCK] Error getting socket error code: %s\n", strerror(retval));
        fprintf(stderr, "%s", errBuf);
        closesocket(socket_fd);

        return false;
    }

    if (error != 0)
    {
        sprintf(errBuf, "[ERR | SOCK] %s\n", strerror(error));
        fprintf(stderr, "%s", errBuf);
        closesocket(socket_fd);

        return false;
    }

    send(socket_fd, static_cast<const char*>(buf), n, flags);

    return true;
}

void setTimeout(const uint32_t s_fd, const int32_t timeoutType, const int32_t secs, const int32_t usec)
{
    const timeval tv{secs, usec};

    setsockopt(s_fd, SOL_SOCKET, timeoutType, reinterpret_cast<const char*>(&tv), sizeof tv);
}

void resetTimeout(const uint32_t s_fd, const int32_t timeoutType)
{
    constexpr timeval tv{0, 0};

    setsockopt(s_fd, SOL_SOCKET, timeoutType, reinterpret_cast<const char*>(&tv), sizeof tv);
}
