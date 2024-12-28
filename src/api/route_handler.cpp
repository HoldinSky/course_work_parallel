#include "route_handler.h"

#include <winsock2.h>

// utilities here

FileIndexer RouteHandler::indexer;

inline bool isLinesDelimiter(char const& ch)
{
    return ch == '\n';
}

void RouteHandler::decideWhatToIndexAndStart(std::string const& requestBody)
{
    for (auto const& pathToIndex : split(requestBody, '\n'))
    {
        RouteHandler::indexer.addToIndex(pathToIndex);
    }
}

void RouteHandler::decideWhatToRemoveAndStart(std::string const& requestBody)
{
    if (requestBody.empty())
    {
        RouteHandler::indexer.removeAllFromIndex();
    }
    else
    {
        for (auto const& pathToIndex : split(requestBody, '\n'))
        {
            RouteHandler::indexer.removeFromIndex(pathToIndex);
        }
    }
}

// methods here

void RouteHandler::addToIndex(std::string const& requestBody)
{
    RouteHandler::decideWhatToIndexAndStart(requestBody);
}

void RouteHandler::removeFromIndex(std::string const& requestBody)
{
    RouteHandler::decideWhatToRemoveAndStart(requestBody);
}

std::set<std::string> RouteHandler::findFilesWithAllWords(std::string const& requestBody, HttpResponse* response)
{
    std::vector<std::string> const wordList = split(requestBody, '\n');
    std::set<std::string> paths;

    auto const err = RouteHandler::indexer.all(wordList, &paths);
    if (err)
    {
        response->error = MapErrorCodeToString(err);
    }

    return paths;
}

std::set<std::string> RouteHandler::findFilesWithAnyWords(std::string const& requestBody, HttpResponse* response)
{
    std::vector<std::string> const wordList = split(requestBody, '\n');
    std::set<std::string> paths;

    auto const err = RouteHandler::indexer.any(wordList, &paths);
    if (err)
    {
        response->error = MapErrorCodeToString(err);
    }

    return paths;
}

std::set<std::string> RouteHandler::reindex()
{
    return RouteHandler::indexer.reindexAll();
}
