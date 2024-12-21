#ifndef CW_FILEINDEXER_H
#define CW_FILEINDEXER_H

#include <map>
#include <string>

class FileIndexer {
private:
    std::map<std::string, std::string> index{};

private:
    int indexFile(const std::string& path);

public:
    int indexDirectory(const std::string& path);
};



#endif // CW_FILEINDEXER_H
