#ifndef CW_API_SERVER_H
#define CW_API_SERVER_H

#include <winsock2.h>
#include <thread>

#include "route_handler.h"
#include "thread_pool/pool.h"

struct acceptedClient
{
    uint32_t socketFd;
    sockaddr_in address;

    bool operator==(const acceptedClient& other) const
    {
        return socketFd == other.socketFd &&
            address.sin_addr.s_addr == other.address.sin_addr.s_addr &&
            address.sin_port == other.address.sin_port &&
            address.sin_family == other.address.sin_family;
    };

    std::size_t operator()(const acceptedClient& client) const noexcept
    {
        return std::hash<uint32_t>()(client.socketFd) ^ std::hash<uint32_t>()(client.address.sin_addr.s_addr);
    }
};

template <>
struct std::hash<acceptedClient>
{
    std::size_t operator()(const acceptedClient& client) const noexcept
    {
        return client(client);
    }
};

namespace srv
{
    int32_t serverRoutine(ThreadPool* pool);

    ThreadTask acceptConnection(const uint32_t& socketHandler, RouteHandler* handler);

    int32_t handleRequest(const acceptedClient& client, RouteHandler* handler);
}

#endif // CW_API_SERVER_H
