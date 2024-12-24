#ifndef CW_SERVER_H
#define CW_SERVER_H

#include <winsock2.h>
#include <thread>

struct acceptedClient {
    uint32_t socket_fd;
    sockaddr_in address;

    bool operator==(const acceptedClient &other) const {
        return socket_fd == other.socket_fd &&
               address.sin_addr.s_addr == other.address.sin_addr.s_addr &&
               address.sin_port == other.address.sin_port &&
               address.sin_family == other.address.sin_family;
    };

    std::size_t operator()(const acceptedClient &client) const noexcept {
        return std::hash<uint32_t>()(client.socket_fd) ^ std::hash<uint32_t>()(client.address.sin_addr.s_addr);
    }
};

template<>
struct std::hash<acceptedClient> {
    std::size_t operator()(const acceptedClient &client) const noexcept {
        return client(client);
    }
};

namespace srv {
    int32_t routine();

    std::pair<acceptedClient, std::thread> acceptConnection(const uint32_t &socketHandler);

    int32_t handleRequest(const acceptedClient &client);
}

#endif // CW_SERVER_H