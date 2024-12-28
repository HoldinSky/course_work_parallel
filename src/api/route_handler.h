#ifndef CW_API_ROUTER_H
#define CW_API_ROUTER_H

#include "http_specific.h"
#include "file_indexer.h"

class RouteHandler
{
private:
    FileIndexer* indexer;

public:
    explicit RouteHandler(FileIndexer *indexer) : indexer(indexer) {}

private:
    [[nodiscard]] int decideWhatToIndexAndStart(std::string const& requestBody) const;
    [[nodiscard]] int decideWhatToRemoveAndStart(std::string const& requestBody) const;

public:
    [[nodiscard]] int addToIndex(std::string const& requestBody) const;
    [[nodiscard]] int removeFromIndex(std::string const& requestBody) const;
    std::set<std::string> findFilesWithAllWords(std::string const& requestBody, HttpResponse* response) const;
    std::set<std::string> findFilesWithAnyWords(std::string const& requestBody, HttpResponse* response) const;
    void reindex() const;
    [[nodiscard]] std::set<std::string> getAllIndexedEntries() const;
};

#endif // CW_API_ROUTER_H
