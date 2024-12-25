#include "router.h"

#include <winsock2.h>

#include "socketUtils.h"

void stringToWordList(char const* const carrier, std::vector<std::string>* const out_wordList)
{
}

void ServerRouter::uploadFile(uint32_t const& socketFd)
{
}

void ServerRouter::deleteFile(uint32_t const& socketFd)
{
}

void ServerRouter::findFilesWithAllWords(uint32_t const& socketFd)
{
    SocketMessageWrapper buffer{};
    ReadFromSocketToWrapper(socketFd, &buffer, 0);

    std::vector<std::string> wordList;
    std::set<std::string> paths;
    stringToWordList(buffer.data, &wordList);

    indexer.all(wordList, &paths);
    SendStringList(socketFd, paths);
}

void ServerRouter::findFilesWithAnyWords(uint32_t const& socketFd)
{
    SocketMessageWrapper buffer{};
    ReadFromSocketToWrapper(socketFd, &buffer, 0);

    std::vector<std::string> wordList;
    std::set<std::string> paths;
    stringToWordList(buffer.data, &wordList);

    indexer.any(wordList, &paths);
    SendStringList(socketFd, paths);
}

void ServerRouter::reindex(uint32_t const& socketFd)
{
}
