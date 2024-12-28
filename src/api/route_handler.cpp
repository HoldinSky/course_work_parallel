#include "route_handler.h"

#include <winsock2.h>

#define REMOVE_ALL_REQUEST_BODY "<all>"

// utilities here

FileIndexer RouteHandler::indexer;

inline bool isLinesDelimiter(char const& ch)
{
    return ch == '\n';
}

int RouteHandler::decideWhatToIndexAndStart(std::string const& requestBody)
{
    if (requestBody.empty())
    {
        return ERROR_EMPTY_BODY;
    }
    for (auto const& pathToIndex : split(requestBody, '\n'))
    {
        RouteHandler::indexer.addToIndex(pathToIndex);
    }

    return 0;
}

int RouteHandler::decideWhatToRemoveAndStart(std::string const& requestBody)
{
    if (requestBody.empty())
    {
        return ERROR_EMPTY_BODY;
    }

    if (requestBody == REMOVE_ALL_REQUEST_BODY)
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

    return 0;
}

// methods here

int RouteHandler::addToIndex(std::string const& requestBody)
{
    return RouteHandler::decideWhatToIndexAndStart(requestBody);
}

int RouteHandler::removeFromIndex(std::string const& requestBody)
{
    return RouteHandler::decideWhatToRemoveAndStart(requestBody);
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
