#ifndef CW_API_ROUTER_H
#define CW_API_ROUTER_H

#include "FileIndexer.h"
#include "socketUtils.h"

#include <cstdint>

class ServerRouter
{
private:
    static FileIndexer indexer;

private:
    static void decideWhatToIndexAndStart(SocketMessageWrapper const& messageFromClient);
    static void decideWhatToRemoveAndStart(SocketMessageWrapper const& messageFromClient);

public:
    static void addToIndex(uint32_t const& socketFd);
    static void removeFromIndex(uint32_t const& socketFd);
    static void findFilesWithAllWords(uint32_t const& socketFd);
    static void findFilesWithAnyWords(uint32_t const& socketFd);
    static void reindex(uint32_t const& socketFd);
};

#endif // CW_API_ROUTER_H
