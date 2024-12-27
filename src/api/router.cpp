#include <winsock2.h>

#include "router.h"
#include "socketUtils.h"
#include "text_utils/fileParser.h"

// utilities here

FileIndexer ServerRouter::indexer;

inline bool isLinesDelimiter(char const& ch)
{
    return ch == '\n';
}

int ServerRouter::decideWhatToIndexAndStart(SocketMessageWrapper const& messageFromClient)
{
    if (messageFromClient.length == 0)
    {
        return ERROR_PATHS_NOT_PROVIDED;
    }

    std::istringstream stream(messageFromClient.data);
    parseInputStreamByWord(stream,
                           [&](char const* w) { ServerRouter::indexer.addToIndex(w); },
                           messageFromClient.length,
                           isLinesDelimiter);

    return 0;
}

void ServerRouter::decideWhatToRemoveAndStart(SocketMessageWrapper const& messageFromClient)
{
    if (messageFromClient.length == 0)
    {
        indexer.removeAllFromIndex();
    }
    else
    {
        std::istringstream stream(messageFromClient.data);
        parseInputStreamByWord(stream,
                               [&](char const* w) { ServerRouter::indexer.removeFromIndex(w); },
                               messageFromClient.length,
                               isLinesDelimiter);
    }
}

void stringToWordList(char const* const carrier, std::vector<std::string>* const out_wordList)
{
    std::istringstream stream(carrier);
    parseInputStreamByWord(
        stream,
        [&](char const* w) { out_wordList->emplace_back(w); },
        0,
        isLinesDelimiter);
}

// methods here

void ServerRouter::addToIndex(uint32_t const& socketFd)
{
    SocketMessageWrapper buffer{};
    ReadFromSocketToWrapper(socketFd, &buffer, 0);

    auto const result = ServerRouter::decideWhatToIndexAndStart(buffer);
    if (result != 0)
    {
        send(socketFd, Commands::error, Commands::errorLen, 0);
        send(socketFd, reinterpret_cast<char const*>(&result), sizeof(result), 0);
    }

    send(socketFd, Commands::processDone, Commands::processDoneLen, 0);
}

void ServerRouter::removeFromIndex(uint32_t const& socketFd)
{
    SocketMessageWrapper buffer{};
    ReadFromSocketToWrapper(socketFd, &buffer, 0);

    ServerRouter::decideWhatToRemoveAndStart(buffer);

    send(socketFd, Commands::processDone, Commands::processDoneLen, 0);
}

void ServerRouter::findFilesWithAllWords(uint32_t const& socketFd)
{
    SocketMessageWrapper buffer{};
    ReadFromSocketToWrapper(socketFd, &buffer, 0);

    std::vector<std::string> wordList;
    std::set<std::string> paths;
    stringToWordList(buffer.data, &wordList);

    auto const err = indexer.all(wordList, &paths);
    if (err == ERROR_CURRENTLY_INDEXING)
    {
        send(socketFd, Commands::currentlyIndexing, Commands::currentlyIndexingLen, 0);
    }
    else
    {
        SendStringList(socketFd, paths);
    }
}

void ServerRouter::findFilesWithAnyWords(uint32_t const& socketFd)
{
    SocketMessageWrapper buffer{};
    ReadFromSocketToWrapper(socketFd, &buffer, 0);

    std::vector<std::string> wordList;
    std::set<std::string> paths;
    stringToWordList(buffer.data, &wordList);

    auto const err = indexer.any(wordList, &paths);
    if (err == ERROR_CURRENTLY_INDEXING)
    {
        send(socketFd, Commands::currentlyIndexing, Commands::currentlyIndexingLen, 0);
    }
    else
    {
        SendStringList(socketFd, paths);
    }
}

void ServerRouter::reindex(uint32_t const& socketFd)
{
    indexer.reindexAll();

    send(socketFd, Commands::processDone, Commands::processDoneLen, 0);
}
