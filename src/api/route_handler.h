#ifndef CW_API_ROUTER_H
#define CW_API_ROUTER_H

#include "http_specific.h"
#include "file_indexer.h"

class RouteHandler
{
private:
    static FileIndexer indexer;

private:
    static int decideWhatToIndexAndStart(std::string const& requestBody);
    static int decideWhatToRemoveAndStart(std::string const& requestBody);

public:
    static int addToIndex(std::string const& requestBody);
    static int removeFromIndex(std::string const& requestBody);
    static std::set<std::string> findFilesWithAllWords(std::string const& requestBody, HttpResponse* response);
    static std::set<std::string> findFilesWithAnyWords(std::string const& requestBody, HttpResponse* response);
    static std::set<std::string> reindex();
};

#endif // CW_API_ROUTER_H
