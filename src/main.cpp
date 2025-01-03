#include <string>
#include <fstream>

#include "file_indexer.h"
#include "api/server.h"
#include "text_utils/file_parser.h"

void initWSA()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed with error: %d\n", WSAGetLastError());
        exit(-1);
    }
}

int getThreadCount(int const argc, char* const * const argv)
{
    int32_t threadCount{};
    if (argc > 1)
    {
        threadCount = strtol(argv[1], nullptr, 10);
    }

    if (threadCount <= 1)
    {
        threadCount = std::max(static_cast<int32_t>(std::thread::hardware_concurrency() / 2), 2);
    }

    return threadCount;
}

void app(int const argc, char* const * const argv)
{
    initWSA();

    ThreadPool threadPool(getThreadCount(argc, argv));

    srv::serverRoutine(&threadPool);
}

int main(int const argc, char* const* const argv)
{
    app(argc, argv);

    return 0;
}
