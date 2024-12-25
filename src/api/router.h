#ifndef CW_API_ROUTER_H
#define CW_API_ROUTER_H
#include <cstdint>

#include "FileIndexer.h"
#include "socketUtils.h"

class ServerRouter
{
private:
    static FileIndexer indexer;

private:

    static void decideWhatToIndexAndStart(SocketMessageWrapper const& messageFromClient);

public:
    static void addToIndex(uint32_t const& socketFd);
    static void removeFromIndex(uint32_t const& socketFd);
    static void findFilesWithAllWords(uint32_t const& socketFd);
    static void findFilesWithAnyWords(uint32_t const& socketFd);
    static void reindex(uint32_t const& socketFd);
};

#endif // CW_API_ROUTER_H
