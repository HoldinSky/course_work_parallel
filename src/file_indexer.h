#ifndef CW_FILE_INDEXER_H
#define CW_FILE_INDEXER_H

#include <set>
#include <unordered_map>
#include <string>
#include <filesystem>

#include "thread_pool/pool.h"

namespace fs = std::filesystem;

class FileIndexer
{
private:
    bool overwriteSave;

public:
    explicit FileIndexer(bool const overwriteStoreFile = true) : overwriteSave(overwriteStoreFile)
    {
        this->readIndexFromCSV();
    };

    ~FileIndexer()
    {
        this->saveIndexToCSV();
    }

private:
    static constexpr auto csvWithStoredIndexPath = R"(D:\prg\cpp\inverted_index\working_data\index.csv)";

    std::unordered_map<std::string, std::set<std::string>> index{};
    std::set<std::string> allFilePaths{};

private:
    std::atomic_bool isCurrentlyIndexing{false};
    std::atomic_bool dangerouslyStopIndexing{false};
    std::shared_mutex indexLock{};

private:
    void addMapping(std::string const& word, std::string const& path);
    void addMapping(std::string const& word, std::set<std::string> const& paths);

    void removeMapping(std::string const& word, std::string const& path);

private:
    int indexFile(fs::path const& path);
    int removeFile(fs::path const& path);

    int indexDirectory(fs::path const& path);
    int removeDirectory(fs::path const& path);

    void indexWord(char const* word, std::string const& path);
    void removeWord(char const* word, std::string const& path);

private:
    std::set<std::string> findFiles(std::string const& word);

    void readIndexFromCSV();
    void indexDefaultDirectory();

public:
    void saveIndexToCSV();
    std::set<std::string> getAllIndexedEntries();

    int addToIndex(std::string const& pathStr);
    int removeFromIndex(std::string const& pathStr);
    void removeAllFromIndex();
    std::set<std::string> reindexAll();

    int all(std::vector<std::string> const& words, std::set<std::string>* out_Paths);
    int any(std::vector<std::string> const& words, std::set<std::string>* out_Paths);
};

#endif // CW_FILE_INDEXER_H
