#include "route_handler.h"
#include "common.h"

#define REMOVE_ALL_REQUEST_BODY "<all>"

// utilities here

int RouteHandler::decideWhatToIndexAndStart(std::string const& requestBody, std::unordered_map<std::string, int>* out_errorCodes) const
{
    if (requestBody.empty())
    {
        return ERROR_EMPTY_BODY;
    }
    for (auto const& pathToIndex : split(requestBody, '\n'))
    {
        if (int const res = indexer->addToIndex(pathToIndex); res != 0)
        {
            out_errorCodes->emplace(pathToIndex, res);
        }
    }

    return 0;
}

int RouteHandler::decideWhatToRemoveAndStart(std::string const& requestBody, std::unordered_map<std::string, int>* out_errorCodes) const
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
            if (int const res = indexer->removeFromIndex(pathToIndex); res != 0)
            {
                out_errorCodes->emplace(pathToIndex, res);
            }
        }
    }

    return 0;
}

// methods here

int RouteHandler::addToIndex(std::string const& requestBody, std::unordered_map<std::string, int>* out_errorCodes) const
{
    return decideWhatToIndexAndStart(requestBody, out_errorCodes);
}

int RouteHandler::removeFromIndex(std::string const& requestBody, std::unordered_map<std::string, int>* out_errorCodes) const
{
    return decideWhatToRemoveAndStart(requestBody, out_errorCodes);
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

int RouteHandler::reindex(std::set<std::string>* out_set) const
{
    return indexer->reindexAll(out_set);
}

int RouteHandler::getAllIndexedEntries(std::set<std::string>* out_paths) const
{
    return indexer->getAllIndexedEntries(out_paths);
}
