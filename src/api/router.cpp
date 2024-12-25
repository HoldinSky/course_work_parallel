#include "router.h"

#include <winsock2.h>

#include "socketUtils.h"

// utilities here

void ServerRouter::decideWhatToIndexAndStart(SocketMessageWrapper const& messageFromClient)
{
    if (messageFromClient.length == 0)
    {
        indexer.reindexAll();
    } else
    {
        // todo get paths list reindex only them
    }
}

void stringToWordList(char const* const carrier, std::vector<std::string>* const out_wordList)
{
    // todo implement
}

// methods here

void ServerRouter::addToIndex(uint32_t const& socketFd)
{
    SocketMessageWrapper buffer{};
    ReadFromSocketToWrapper(socketFd, &buffer, 0);

    ServerRouter::decideWhatToIndexAndStart(buffer);

    send(socketFd, Commands::processDone, Commands::processDoneLen, 0);
}

void ServerRouter::removeFromIndex(uint32_t const& socketFd)
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
