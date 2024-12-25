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
    static constexpr auto defaultIndexFile = R"(D:\prg\cpp\inverted_index\working_data\index.csv)";

    bool overwriteSave;

    std::unordered_map<std::string, std::set<std::string> > index{};
    std::atomic_bool isCurrentlyIndexing{false};

    std::shared_mutex indexLock{};

private:
    void addMapping(const std::string &word, const std::string &path);

    void addMapping(const std::string &word, const std::set<std::string> &paths);

private:
    int indexFile(const std::string &path);

    int indexDirectory(const std::string &path);

    void indexWord(const char *word, const std::string &path);

private:
    std::set<std::string> findFiles(const std::string &word);

    void saveIndexToFile();

    void readIndexFromFile();

public:
    int addToIndex(const std::string &path);

    int all(std::vector<std::string> const &words, std::set<std::string> *const out_Paths);
    int any(std::vector<std::string> const &words, std::set<std::string> *const out_Paths);
};


#endif // CW_FILEINDEXER_H
