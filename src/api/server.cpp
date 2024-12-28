#include "server.h"
#include "socket_utils.h"
#include "route_handler.h"
#include "http_specific.h"

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <unordered_map>

// utilities function here
std::atomic_uint64_t taskIdCounter = 10;

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

ThreadTask srv::acceptConnection(const uint32_t& socketHandler, RouteHandler const& handler)
{
    auto accepted = internalAccept(socketHandler);

    {
        // it is here because inet_ntoa is not thread-safe, it uses static internal buffer
        exclusiveLock _(stdoutLock);
        printf("[INFO | HTTP] Accepted connection from %s:%d\n",
               inet_ntoa(accepted.address.sin_addr),
               ntohs(accepted.address.sin_port));
    }

    ThreadTask task{};
    task.id = taskIdCounter.fetch_add(1);
    task.action = [=]
    {
        handleRequest(accepted, handler);
    };

    return task;
}

// main part here

int32_t srv::serverRoutine(ThreadPool* pool)
{
    const uint32_t mainLoopSocket_d = createAndOpenSocket(DEFAULT_PORT);

    FileIndexer indexer{};
    RouteHandler const handler(&indexer);

    printf("[INFO] Server is up and running on :%d\n", DEFAULT_PORT);

    ThreadTask saveIndexTask{};
    saveIndexTask.id = 0;
    saveIndexTask.action = [&]()
    {
        while (true)
        {
            indexer.saveIndexToCSV();
            std::this_thread::sleep_for(std::chrono::minutes(5));
        }
    };
    pool->scheduleTask(saveIndexTask);

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
            auto task = acceptConnection(mainLoopSocket_d, handler);
            pool->scheduleTask(task);
        }
    }

    closesocket(mainLoopSocket_d);
    return 0;
}

void routeRequest(uint32_t const& socketFd, RouteHandler const& handler)
{
    char buf[BYTES_IN_1MB]{};
    ssize_t bytesRead = recv(socketFd, buf, BYTES_IN_1MB, 0);
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

    std::set<std::string> returnSet;
    auto const route = request.path.c_str();
    bool isWrongMethod = false;
    int result{};
    if (strcmp(route, RequestPath::addToIndex) == 0)
    {
        result = handler.addToIndex(request.body);
        isWrongMethod = request.method != Method::POST;
    }
    else if (strcmp(route, RequestPath::removeFromIndex) == 0)
    {
        result = handler.removeFromIndex(request.body);
        isWrongMethod = request.method != Method::POST;
    }
    else if (strcmp(route, RequestPath::filesWithAllWords) == 0)
    {
        returnSet = handler.findFilesWithAllWords(request.body, &response);
        isWrongMethod = request.method != Method::POST;
    }
    else if (strcmp(route, RequestPath::filesWithAnyWord) == 0)
    {
        returnSet = handler.findFilesWithAnyWords(request.body, &response);
        isWrongMethod = request.method != Method::POST;
    }
    else if (strcmp(route, RequestPath::reindex) == 0)
    {
        handler.reindex();
        isWrongMethod = request.method != Method::POST;
    }
    else if (strcmp(route, RequestPath::getAllIndexed) == 0)
    {
        returnSet = handler.getAllIndexedEntries();
        isWrongMethod = request.method != Method::GET;
    }
    else
    {
        response.error = "Unknown request route";
        response.topLine = TOP_LINE_NOT_FOUND;
    }

    if (response.error.empty())
    {
        if (result != 0)
        {
            response.error = MapErrorCodeToString(result);
            response.topLine = TOP_LINE_BAD_REQUEST;
        }
        else if (isWrongMethod)
        {
            response.error = "Invalid request method";
            response.topLine = TOP_LINE_BAD_REQUEST;
        }
        else
        {
            response.topLine = TOP_LINE_200;
            std::stringstream bodyStream;
            for (auto const& path : returnSet)
            {
                bodyStream << path << "\r\n";
            }
            response.body = bodyStream.str();
        }
    }

    auto const responseStr = composeResponse(request, response);
    send(socketFd, responseStr.c_str(), static_cast<int32_t>(responseStr.length()), 0);

    resetTimeout(socketFd, SO_RCVTIMEO);
}

int32_t srv::handleRequest(const acceptedClient& client, RouteHandler const& handler)
{
    // accept task input in 60 seconds timeout
    setTimeout(client.socketFd, SO_RCVTIMEO, 60);

    routeRequest(client.socketFd, handler);

    closesocket(client.socketFd);

    return 0;
}
