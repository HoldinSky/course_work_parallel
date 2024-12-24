#include "common.h"

#include <winsock2.h>
#include <csignal>

void printErrorAndHalt(const char *const msg) {
    fprintf(stderr, "%s", msg);
    exit(-1);
}

void populateMessageWithBytes(
    char *buf,
    const uint32_t maxSize,
    uint32_t &currentSize,
    size_t dataSize,
    const char *ptrToData
) {
    while (dataSize--) {
        buf[currentSize++] = *ptrToData++;
        if (currentSize == maxSize - 1) break;
    }

    buf[currentSize] = '\0';
}


/// 'current_size' becomes the size of 'data' including '\0'
void appendToMessage(char *buf, uint32_t &current_size, const uint32_t max_size, const char *const data) {
    const auto ptr = const_cast<char *>(data);
    const size_t data_size = strlen(data);

    populateMessageWithBytes(buf, max_size, current_size, data_size, ptr);
}

/// rewrites passed message 'data' at the beginning of 'buf'.
/// 'current_size' becomes the size of 'data' including '\0'
void startMessage(char *buf, uint32_t &current_size, const uint32_t max_size, const char *const data) {
    current_size = 0;
    appendToMessage(buf, current_size, max_size, data);
}

/// closes socket in case of error
bool safeSend(const uint32_t socket_fd, const void *buf, const int32_t n, const int32_t flags) {
    char error = 0;
    int len = sizeof(error);
    const int retval = getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, &len);

    char errBuf[100];
    if (retval != 0) {
        sprintf(errBuf, "[ERR | SOCK] Error getting socket error code: %s\n", strerror(retval));
        fprintf(stderr, "%s", errBuf);
        closesocket(socket_fd);

        return false;
    }

    if (error != 0) {
        sprintf(errBuf, "[ERR | SOCK] %s\n", strerror(error));
        fprintf(stderr, "%s", errBuf);
        closesocket(socket_fd);

        return false;
    }

    send(socket_fd, static_cast<const char*>(buf), n, flags);

    return true;
}

void setTimeout(const uint32_t s_fd, const int32_t timeoutType, const int32_t secs, const int32_t usec) {
    const timeval tv{secs, usec};

    setsockopt(s_fd, SOL_SOCKET, timeoutType, reinterpret_cast<const char *>(&tv), sizeof tv);
}

void resetTimeout(const int32_t s_fd, const int32_t timeoutType) {
    constexpr timeval tv{0, 0};

    setsockopt(s_fd, SOL_SOCKET, timeoutType, reinterpret_cast<const char *>(&tv), sizeof tv);
}
