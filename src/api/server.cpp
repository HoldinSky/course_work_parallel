#include "server.h"
#include "common.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <unordered_map>

namespace srv {
    uint32_t createAndOpenSocket(uint16_t port);
}

int32_t srv::routine() {
    const uint32_t mainLoopSocket_d = createAndOpenSocket(DEFAULT_PORT);

    std::unordered_map<acceptedClient, std::thread> client_thread_handlers{};

    printf("[INFO] Server is up and running on :%d\n", DEFAULT_PORT);

    std::thread _([&]() {
        [[noreturn]]
        while (true) {
            std::this_thread::sleep_for(std::chrono::minutes(1));
            for (auto &[client, thread]: client_thread_handlers) {
                if (thread.joinable()) {
                    thread.join();
                    client_thread_handlers.erase(client);
                }
            }
        }
    });

    fd_set readfds;
    constexpr timeval timeout{1, 0}; // timeout 1 second

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(mainLoopSocket_d, &readfds);

        const int rcode = select(0, &readfds, nullptr, nullptr, &timeout); // '0' for Windows fd_set size

        if (rcode == SOCKET_ERROR) {
            fprintf(stderr, "[ERR | SOCK] Select error: %d\n", WSAGetLastError());
            break;
        }

        if (FD_ISSET(mainLoopSocket_d, &readfds)) {
            auto connection = acceptConnection(mainLoopSocket_d);

            client_thread_handlers.insert(std::move(connection));
        }
    }

    for (auto &[_, thread]: client_thread_handlers) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    closesocket(mainLoopSocket_d);
    return 0;
}

uint32_t srv::createAndOpenSocket(const uint16_t port) {
    sockaddr_in server{};

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    const uint32_t socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    const int64_t returnCode = bind(socketDescriptor, reinterpret_cast<sockaddr *>(&server), sizeof(server));
    if (returnCode == -1) {
        closesocket(socketDescriptor);
        printErrorAndHalt("server :: Failed to bind socket");
    }

    listen(socketDescriptor, 5);

    return socketDescriptor;
}

acceptedClient internalAccept(const uint32_t &socket_handler) {
    acceptedClient accepted{};

    int32_t client_len = sizeof(accepted.address);
    accepted.socket_fd = accept(socket_handler, reinterpret_cast<sockaddr *>(&accepted.address), &client_len);

    if (accepted.socket_fd == -1) {
        closesocket(socket_handler);
        printErrorAndHalt("server :: Failed to accept socket connection\n");
    }

    return accepted;
}

std::pair<acceptedClient, std::thread> srv::acceptConnection(const uint32_t &socketHandler) {
    auto accepted = internalAccept(socketHandler);

    printf("[INFO | SOCK] Successfully accepted connection\n");

    std::thread t(&srv::handleRequest, accepted);
    return std::make_pair(accepted, std::move(t));
}

int32_t srv::handleRequest(const acceptedClient &client) {
    int32_t msg_pos = 0;
    char buffer[BUFFER_SIZE];


    startMessage(buffer, ++msg_pos, BUFFER_SIZE, Commands::readyReceiveData);
    if (!safeSend(client.socket_fd, buffer, msg_pos, 0)) {
        return -1;
    }

    // accept task input in 300 seconds timeout
    setTimeout(client.socket_fd, SO_RCVTIMEO, 300);

    closesocket(client.socket_fd);

    return 0;
}
