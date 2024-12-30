#include <string>
#include <fstream>

#include "file_indexer.h"
#include "api/server.h"
#include "text_utils/file_parser.h"

int main(int argc, char** argv)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    int32_t threadCount{};
    if (argc > 1)
    {
        threadCount = strtol(argv[1], nullptr, 10);
    }

    if (threadCount <= 1)
    {
        threadCount = std::max(static_cast<int32_t>(std::thread::hardware_concurrency() / 2), 2);
    }

    ThreadPool threadPool(threadCount);

    srv::serverRoutine(&threadPool);

    return 0;
}
