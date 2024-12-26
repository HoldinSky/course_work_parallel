#ifndef CW_FILEINDEXER_H
#define CW_FILEINDEXER_H

#define ERROR_FILE_CANNOT_BE_OPENED 270

#define ERROR_CURRENTLY_INDEXING 271
#define ERROR_PATH_DOES_NOT_EXIST 272
#define ERROR_FILE_ALREADY_INDEXED 273
#define ERROR_FILE_ALREADY_NOT_INDEXED 274
#define ERROR_WORDS_NOT_PROVIDED 275

#include "thread_pool/pool.h"

#include <set>
#include <unordered_map>
#include <string>
#include <filesystem>


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
    static constexpr auto defaultIndexFile = R"(D:\prg\cpp\inverted_index\working_data\index.csv)";

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

    void saveIndexToCSV();

    void readIndexFromCSV();

public:
    int addToIndex(std::string const& pathStr);
    int removeFromIndex(std::string const& pathStr);
    void removeAllFromIndex();
    void reindexAll();

    int all(std::vector<std::string> const& words, std::set<std::string>* const out_Paths);
    int any(std::vector<std::string> const& words, std::set<std::string>* const out_Paths);
};


#endif // CW_FILEINDEXER_H
