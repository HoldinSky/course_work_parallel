#ifndef CW_API_ROUTER_H
#define CW_API_ROUTER_H
#include <cstdint>

#include "FileIndexer.h"

class ServerRouter
{
private:
    static FileIndexer indexer;

public:
    static void uploadFile(uint32_t const& socketFd);
    static void deleteFile(uint32_t const& socketFd);
    static void findFilesWithAllWords(uint32_t const& socketFd);
    static void findFilesWithAnyWords(uint32_t const& socketFd);
    static void reindex(uint32_t const& socketFd);
};

#endif // CW_API_ROUTER_H
