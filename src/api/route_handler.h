#ifndef CW_API_ROUTER_H
#define CW_API_ROUTER_H

#include "file_indexer.h"

class RouteHandler
{
private:
    FileIndexer* indexer;

public:
    explicit RouteHandler(FileIndexer* indexer) : indexer(indexer)
    {
    }

private:
    int decideWhatToIndexAndStart(std::string const& requestBody, std::unordered_map<std::string, int>* out_errorCodes) const;
    int decideWhatToRemoveAndStart(std::string const& requestBody, std::unordered_map<std::string, int>* out_errorCodes) const;

public:
    int addToIndex(std::string const& requestBody, std::unordered_map<std::string, int>* out_errorCodes) const;
    int removeFromIndex(std::string const& requestBody, std::unordered_map<std::string, int>* out_errorCodes) const;
    int findFilesWithAllWords(std::string const& requestBody, std::set<std::string>* out_paths) const;
    int findFilesWithAnyWords(std::string const& requestBody, std::set<std::string>* out_paths) const;
    int reindex(std::set<std::string>* out_set) const;
    [[nodiscard]] int getAllIndexedEntries(std::set<std::string>* out_paths) const;
};

#endif // CW_API_ROUTER_H
