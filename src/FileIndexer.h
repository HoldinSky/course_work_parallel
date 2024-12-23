#ifndef CW_FILEINDEXER_H
#define CW_FILEINDEXER_H

#include <set>
#include <unordered_map>
#include <string>

#include "thread_pool/pool.h"

class FileIndexer {
private:
    std::unordered_map<std::string, std::set<std::string>> index{};

private:
    int indexFile(const std::string& path);
    void indexWord(const char* word, const std::string& path);

public:
    int indexDirectory(const std::string& path);
    std::set<std::string> findFiles(const std::string& word);
};



#endif // CW_FILEINDEXER_H
