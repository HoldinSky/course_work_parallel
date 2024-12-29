#include "socket_utils.h"

#include <winsock2.h>
#include <csignal>

void printErrorAndHalt(char const* const msg)
{
    fprintf(stderr, "%s", msg);
    exit(-1);
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
