#include <string>
#include <fstream>

#include "file_indexer.h"
#include "api/server.h"
#include "text_utils/file_parser.h"

void initWSA(int argc, char** argv)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed with error: %d\n", WSAGetLastError());
        exit(-1);
    }
}

int getThreadCount(int argc, char** argv)
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

void app(int argc, char** argv)
{
    initWSA(argc, argv);

    ThreadPool threadPool(getThreadCount(argc, argv));

    srv::serverRoutine(&threadPool);
}

void indexBuildingTimeTest()
{
    constexpr int32_t testsCount = 10;
    int64_t timeSum{};

    for (int32_t i = 1; i <= testsCount; ++i)
    {
        auto const timeSpent = measureTimeMillis([&]
        {
            FileIndexer indexer;
            indexer.indexDefaultDirectory();
        });

        printf("%d. Index fully built in %lld ms\n", i, timeSpent.count());
        timeSum += timeSpent.count();
    }

    printf("Average index build time: %lld\n", timeSum / testsCount);
}

int main(int argc, char** argv)
{
    indexBuildingTimeTest();

    return 0;
}
