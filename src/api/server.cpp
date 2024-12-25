#include "server.h"
#include "socketUtils.h"
#include "router.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <unordered_map>

// utilities function here

uint32_t createAndOpenSocket(const uint16_t port)
{
    sockaddr_in server{};

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    const uint32_t socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    const int64_t returnCode = bind(socketDescriptor, reinterpret_cast<sockaddr*>(&server), sizeof(server));
    if (returnCode == -1)
    {
        closesocket(socketDescriptor);
        printErrorAndHalt("server :: Failed to bind socket");
    }

    listen(socketDescriptor, 5);

    return socketDescriptor;
}

acceptedClient internalAccept(const uint32_t& socket_handler)
{
    acceptedClient accepted{};

    int32_t client_len = sizeof(accepted.address);
    accepted.socketFd = accept(socket_handler, reinterpret_cast<sockaddr*>(&accepted.address), &client_len);

    if (accepted.socketFd == -1)
    {
        closesocket(socket_handler);
        printErrorAndHalt("server :: Failed to accept socket connection\n");
    }

    return accepted;
}

std::pair<acceptedClient, std::thread> srv::acceptConnection(const uint32_t& socketHandler)
{
    auto accepted = internalAccept(socketHandler);

    printf("[INFO | SOCK] Successfully accepted connection\n");

    std::thread t(&srv::handleRequest, accepted);
    return std::make_pair(accepted, std::move(t));
}

// main part here

int32_t srv::serverRoutine()
{
    const uint32_t mainLoopSocket_d = createAndOpenSocket(DEFAULT_PORT);

    std::unordered_map<acceptedClient, std::thread> clientHandlerThreads{};

    printf("[INFO] Server is up and running on :%d\n", DEFAULT_PORT);

    std::thread _([&]()
    {
        [[noreturn]]
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(15));
            for (auto& [client, thread] : clientHandlerThreads)
            {
                if (thread.joinable())
                {
                    thread.join();
                    clientHandlerThreads.erase(client);
                }
            }
        }
    });

    fd_set readfds;
    constexpr timeval timeout{1, 0}; // timeout 1 second

    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(mainLoopSocket_d, &readfds);

        const int rcode = select(0, &readfds, nullptr, nullptr, &timeout); // '0' for Windows fd_set size

        if (rcode == SOCKET_ERROR)
        {
            fprintf(stderr, "[ERR | SOCK] Select error: %d\n", WSAGetLastError());
            break;
        }

        if (FD_ISSET(mainLoopSocket_d, &readfds))
        {
            auto connection = acceptConnection(mainLoopSocket_d);

            clientHandlerThreads.insert(std::move(connection));
        }
    }

    for (auto& [_, thread] : clientHandlerThreads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    closesocket(mainLoopSocket_d);
    return 0;
}

void routeRequest(uint32_t const& socketFd)
{
    static constexpr int routingBufferSize = 1024;
    SocketMessageWrapper buffer{};

    int rcode = recv(socketFd, reinterpret_cast<char*>(&buffer.length), sizeof(buffer.length), 0);
    if (rcode <= 0)
    {
        return;
    }

    rcode = recv(socketFd, buffer.data, buffer.length, 0);
    if (rcode <= 0)
    {
        return;
    }

    auto const route = std::string(buffer.data);
    switch (route)
    {
    case ServerRoute::addToIndex:
        ServerRouter::addToIndex(socketFd);
        break;
    case ServerRoute::removeFromIndex:
        ServerRouter::removeFromIndex(socketFd);
        break;
    case ServerRoute::filesWithAllWords:
        ServerRouter::findFilesWithAllWords(socketFd);
        break;
    case ServerRoute::filesWithAnyWord:
        ServerRouter::findFilesWithAnyWords(socketFd);
        break;
    case ServerRoute::reindex:
        ServerRouter::reindex(socketFd);
        break;
    default:
        auto const msg = "Unknown route";
        send(socketFd, msg, strlen(msg), 0);
        return;
    }
}

int32_t srv::handleRequest(const acceptedClient& client)
{
    int32_t msg_pos = 0;
    char buffer[BUFFER_SIZE];

    startMessage(buffer, ++msg_pos, BUFFER_SIZE, Commands::listeningTheSocket);
    if (!safeSend(client.socketFd, buffer, msg_pos, 0))
    {
        return -1;
    }

    // accept task input in 10 seconds timeout
    setTimeout(client.socketFd, SO_RCVTIMEO, 15);

    routeRequest(client.socketFd);

    closesocket(client.socketFd);

    return 0;
}
