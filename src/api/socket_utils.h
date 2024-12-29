#ifndef CW_API_SOCKET_UTILS_H
#define CW_API_SOCKET_UTILS_H

#include "common.h"

#include <cstdint>

#define DEFAULT_PORT 2772
#define SERVER_ADDR "127.0.0.1"

void printErrorAndHalt(const char* msg);

void setTimeout(uint32_t s_fd, int32_t timeoutType, int32_t secs, int32_t usec = 0);

void resetTimeout(uint32_t s_fd, int32_t timeoutType);

#endif // CW_API_SOCKET_UTILS_H
