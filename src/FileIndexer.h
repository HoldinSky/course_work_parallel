#ifndef CW_FILEINDEXER_H
#define CW_FILEINDEXER_H

#include <set>
#include <unordered_map>
#include <string>

#include "thread_pool/pool.h"

class FileIndexer {
public:
    explicit FileIndexer(const bool overwriteStoreFile = true) : overwriteSave(overwriteStoreFile) {
        this->readIndexFromFile();
    };

    ~FileIndexer() {
        this->saveIndexToFile();
    }

private:
    static constexpr const char *defaultIndexFile = R"(D:\prg\cpp\inverted_index\working_data\index.csv)";

    std::unordered_map<std::string, std::set<std::string> > index{};
    bool overwriteSave;

private:
    void addMapping(const std::string &word, const std::string &path);

    void addMapping(const std::string &word, const std::set<std::string> &paths);

private:
    int indexFile(const std::string &path);

    void indexWord(const char *word, const std::string &path);

private:
    std::set<std::string> findFiles(const std::string &word);

    void saveIndexToFile();

    void readIndexFromFile();

public:
    int indexDirectory(const std::string &path);

    std::set<std::string> all(const std::vector<std::string> &words);

    std::set<std::string> any(const std::vector<std::string> &words);
};


#endif // CW_FILEINDEXER_H
