#include "route_handler.h"

#include <winsock2.h>

#define REMOVE_ALL_REQUEST_BODY "<all>"

// utilities here

int RouteHandler::decideWhatToIndexAndStart(std::string const& requestBody) const
{
    if (requestBody.empty())
    {
        return ERROR_EMPTY_BODY;
    }
    for (auto const& pathToIndex : split(requestBody, '\n'))
    {
        indexer->addToIndex(pathToIndex);
    }

    return 0;
}

int RouteHandler::decideWhatToRemoveAndStart(std::string const& requestBody) const
{
    if (requestBody.empty())
    {
        return ERROR_EMPTY_BODY;
    }

    if (requestBody == REMOVE_ALL_REQUEST_BODY)
    {
        indexer->removeAllFromIndex();
    }
    else
    {
        for (auto const& pathToIndex : split(requestBody, '\n'))
        {
            indexer->removeFromIndex(pathToIndex);
        }
    }

    return 0;
}

// methods here

int RouteHandler::addToIndex(std::string const& requestBody) const
{
    return RouteHandler::decideWhatToIndexAndStart(requestBody);
}

int RouteHandler::removeFromIndex(std::string const& requestBody) const
{
    return RouteHandler::decideWhatToRemoveAndStart(requestBody);
}

int RouteHandler::findFilesWithAllWords(std::string const& requestBody, std::set<std::string>* out_paths) const
{
    std::vector<std::string> const wordList = split(requestBody, ' ');

    return indexer->all(wordList, out_paths);
}

int RouteHandler::findFilesWithAnyWords(std::string const& requestBody, std::set<std::string>* out_paths) const
{
    std::vector<std::string> const wordList = split(requestBody, ' ');

    return indexer->any(wordList, out_paths);
}

void RouteHandler::reindex() const
{
    indexer->reindexAll();
}

std::set<std::string> RouteHandler::getAllIndexedEntries() const
{
    return indexer->getAllIndexedEntries();
}
