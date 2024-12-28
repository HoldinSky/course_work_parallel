#include "server.h"
#include "socket_utils.h"
#include "route_handler.h"
#include "http_specific.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <unordered_map>

// utilities function here

uint32_t createAndOpenSocket(uint16_t const port)
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

    {
        // it is here because inet_ntoa is not thread-safe, it uses static internal buffer
        exclusiveLock _(stdoutLock);
        printf("[INFO | SOCK] Successfully accepted connection from %s:%d\n",
               inet_ntoa(accepted.address.sin_addr),
               ntohs(accepted.address.sin_port));
    }

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

        int const rcode = select(0, &readfds, nullptr, nullptr, &timeout); // '0' for Windows fd_set size

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
    char buf[BUFFER_SIZE]{};

    ssize_t bytesRead = recv(socketFd, buf, BUFFER_SIZE, 0);
    if (bytesRead == 0)
    {
        fprintf(stderr, "[ERR | SOCK] Connection closed by peer\n");
        return;
    }
    else if (bytesRead == -1)
    {
        fprintf(stderr, "[ERR | SOCK] Failed to receive data from client or timeout was reached\n");
        return;
    }

    HttpRequest request{};
    HttpResponse response{};
    parseRequest(buf, &request, &response);
    if (!response.error.empty())
    {
        auto const responseStr = composeResponse(request, response);
        send(socketFd, responseStr.c_str(), static_cast<int32_t>(responseStr.size()), 0);
        return;
    }

    HttpTopLine topLine{};
    parseHttpTopLine(request.topLine, &topLine, &response);
    if (!response.error.empty())
    {
        auto const responseStr = composeResponse(request, response);
        send(socketFd, responseStr.c_str(), static_cast<int32_t>(responseStr.size()), 0);
        return;
    }

    std::set<std::string> returnSet;
    auto const route = topLine.requestPath.c_str();
    if (strcmp(route, RequestPath::addToIndex) == 0)
    {
        RouteHandler::addToIndex(request.body);
    }
    else if (strcmp(route, RequestPath::removeFromIndex) == 0)
    {
        RouteHandler::removeFromIndex(request.body);
    }
    else if (strcmp(route, RequestPath::filesWithAllWords) == 0)
    {
        returnSet = RouteHandler::findFilesWithAllWords(request.body, &response);
    }
    else if (strcmp(route, RequestPath::filesWithAnyWord) == 0)
    {
        returnSet = RouteHandler::findFilesWithAnyWords(request.body, &response);
    }
    else if (strcmp(route, RequestPath::reindex) == 0)
    {
        returnSet = RouteHandler::reindex();
    }
    else
    {
        response.error = "Unknown request route";
        response.topLine = TOP_LINE_NOT_FOUND;
    }

    std::string const responseStr = composeResponse(request, response, returnSet);
    send(socketFd, responseStr.c_str(), static_cast<int32_t>(responseStr.length()), 0);

    resetTimeout(socketFd, SO_RCVTIMEO);
}

int32_t srv::handleRequest(const acceptedClient& client)
{
    // accept task input in 60 seconds timeout
    setTimeout(client.socketFd, SO_RCVTIMEO, 60);

    routeRequest(client.socketFd);

    closesocket(client.socketFd);

    return 0;
}
