#include "file_indexer.h"
#include "text_utils/file_parser.h"
#include "common.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

// utilities

std::string const dataRootPath = R"(D:\prg\cpp\inverted_index\data)";

void FileIndexer::indexDefaultDirectory()
{
    for (const auto& entry : fs::directory_iterator(dataRootPath))
    {
        if (entry.is_directory())
        {
            this->indexDirectory(entry.path());
        }
    }
}

void FileIndexer::addMapping(const std::string& word, const std::string& path)
{
    std::set<std::string> pathSet;
    try
    {
        pathSet = this->index.at(word);
        pathSet.insert(path);
    }
    catch (const std::out_of_range& _)
    {
        pathSet.insert(path);
    }
    this->allIndexedPaths.emplace(path);

    this->index.insert_or_assign(word, pathSet);
}

void FileIndexer::addMapping(const std::string& word, const std::set<std::string>& paths)
{
    std::set<std::string> pathSet = paths;
    try
    {
        pathSet = this->index.at(word);
        for (const auto& path : paths)
        {
            pathSet.insert(path);
        }
    }
    catch (const std::out_of_range& _)
    {
    }

    for (const auto& path : pathSet)
    {
        this->allIndexedPaths.emplace(path);
    }
    this->index.insert_or_assign(word, pathSet);
}

void FileIndexer::removeMapping(std::string const& word, std::string const& path)
{
    std::set<std::string> pathSet;
    try
    {
        pathSet = this->index.at(word);
        pathSet.erase(path);

        if (pathSet.empty())
        {
            this->index.erase(word);
        }
    }
    catch (std::out_of_range const& _)
    {
    }
    this->allIndexedPaths.erase(path);

    this->index.insert_or_assign(word, pathSet);
}

// private here

int FileIndexer::indexDirectory(fs::path const& path)
{
    for (const auto& entry : fs::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            this->indexDirectory(entry.path());
        }
        else
        {
            this->indexFile(entry.path().string());
        }
    }

    return 0;
}

int FileIndexer::removeDirectory(fs::path const& path)
{
    for (auto const& entry : fs::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            this->removeDirectory(entry.path());
        }
        else
        {
            this->removeFile(entry.path().string());
        }
    }

    return 0;
}

int FileIndexer::indexFile(std::string const& path)
{
    std::ifstream file(path);
    if (!file)
    {
        fprintf(stderr, "[ERR] Could not open file %s\n", path.c_str());
        return ERROR_FILE_CANNOT_BE_OPENED;
    }

    parseInputStreamByWord(file, [this, path](char const* word) { this->addMapping(word, path); });

    return 0;
}

int FileIndexer::removeFile(std::string const& path)
{
    std::ifstream file(path);
    if (!file)
    {
        fprintf(stderr, "[ERR] Could not open file %s\n", path.c_str());
        return ERROR_FILE_CANNOT_BE_OPENED;
    }

    parseInputStreamByWord(file, [this, path](char const* word) { removeMapping(word, path); });

    return 0;
}

std::set<std::string> FileIndexer::findFiles(const std::string& word)
{
    try
    {
        return this->index.at(word);
    }
    catch (const std::out_of_range& _)
    {
        return {};
    }
}

void FileIndexer::readIndexFromCSV()
{
    if (!fs::exists(csvWithStoredIndexPath) || fs::file_size(csvWithStoredIndexPath) < BYTES_IN_1KB * 100)
    {
        this->indexDefaultDirectory();
        this->saveIndexToCSV();
        return;
    }

    std::ifstream file(csvWithStoredIndexPath, std::ios::in);
    if (!file || !file.is_open())
    {
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string cell;
        std::set<std::string> storedPaths;

        std::getline(ss, cell, ',');
        const auto word = cell;

        while (std::getline(ss, cell, ','))
        {
            storedPaths.insert(cell);
        }

        this->addMapping(word, storedPaths);
    }
}

void FileIndexer::saveIndexToCSV()
{
    const bool exists = fs::exists(csvWithStoredIndexPath);
    if (exists && !this->overwriteCsv)
    {
        return;
    }

    if (!exists)
    {
        fs::path targetFilePath(csvWithStoredIndexPath);

        if (targetFilePath.has_parent_path())
        {
            fs::create_directories(targetFilePath.parent_path());
        }
    }
    else
    {
        fs::remove(csvWithStoredIndexPath);
    }

    std::ofstream file(csvWithStoredIndexPath, std::ios::out);
    if (!file || !file.is_open())
    {
        return;
    }

    std::string line;
    std::stringstream ss(line);

    exclusiveLock _(this->indexLock);
    for (const auto& [word, paths] : this->index)
    {
        ss << word;
        for (const auto& path : paths)
        {
            ss << ',' << path;
        }
        ss << '\n';
        file << ss.str();
        ss.str(std::string());
    }
}

// public here

int FileIndexer::addToIndex(const std::string& pathStr)
{
    auto const path = fs::path(pathStr);
    if (!fs::exists(path))
    {
        fprintf(stderr, "[ERR] Path %ls does not exist\n", path.c_str());
        return ERROR_PATH_DOES_NOT_EXIST;
    }

    this->isCurrentlyIndexing.exchange(true);
    int rcode;

    exclusiveLock _(this->indexLock);
    if (fs::is_directory(path))
    {
        rcode = this->indexDirectory(path);
    }
    else
    {
        if (this->allIndexedPaths.contains(pathStr))
        {
            this->isCurrentlyIndexing.exchange(false);
            return ERROR_PATH_ALREADY_INDEXED;
        }
        rcode = this->indexFile(pathStr);
    }

    this->isCurrentlyIndexing.exchange(false);

    return rcode;
}

int FileIndexer::removeFromIndex(std::string const& pathStr)
{
    auto const path = fs::path(pathStr);
    if (!fs::exists(path))
    {
        fprintf(stderr, "[ERR] Path %ls does not exist\n", path.c_str());
        return ERROR_PATH_DOES_NOT_EXIST;
    }

    this->isCurrentlyIndexing.exchange(true);

    exclusiveLock _(this->indexLock);
    int rcode;
    if (fs::is_directory(path))
    {
        rcode = this->removeDirectory(path);
    }
    else
    {
        if (!this->allIndexedPaths.contains(pathStr))
        {
            this->isCurrentlyIndexing.exchange(false);
            return ERROR_PATH_ALREADY_NOT_INDEXED;
        }
        rcode = this->removeFile(pathStr);
    }
    this->isCurrentlyIndexing.exchange(false);

    return rcode;
}

void FileIndexer::removeAllFromIndex()
{
    exclusiveLock _(this->indexLock);

    this->index.clear();
    this->allIndexedPaths.clear();
}

int FileIndexer::reindexAll(std::set<std::string>* out_set)
{
    if (this->isCurrentlyIndexing.load())
    {
        return ERROR_CURRENTLY_INDEXING;
    }
    this->isCurrentlyIndexing.store(true);

    exclusiveLock _(this->indexLock);
    this->index.clear();
    for (auto const& entry : this->allIndexedPaths)
    {
        auto const path = fs::path(entry);
        if (fs::is_directory(path))
        {
            this->indexDirectory(path);
        }
        else
        {
            this->indexFile(entry);
        }
    }

    this->isCurrentlyIndexing.store(false);
    *out_set = this->allIndexedPaths;

    return 0;
}


int FileIndexer::all(const std::vector<std::string>& words, std::set<std::string>* const out_Paths)
{
    if (this->isCurrentlyIndexing.load())
    {
        return ERROR_CURRENTLY_INDEXING;
    }

    sharedLock _(this->indexLock);
    if (words.empty())
    {
        return ERROR_WORDS_NOT_PROVIDED;
    }

    *out_Paths = this->findFiles(words[0]);
    std::set<std::string> intersection;
    for (uint64_t i = 1; i < words.size(); ++i)
    {
        const auto& set = this->findFiles(words[i]);

        std::set_intersection(
            out_Paths->begin(), out_Paths->end(),
            set.begin(), set.end(),
            std::inserter(intersection, intersection.begin())
        );
        *out_Paths = intersection;
        intersection.clear();
    }

    return 0;
}

int FileIndexer::any(const std::vector<std::string>& words, std::set<std::string>* const out_Paths)
{
    if (this->isCurrentlyIndexing.load())
    {
        return ERROR_CURRENTLY_INDEXING;
    }

    sharedLock _(this->indexLock);
    if (words.empty())
    {
        return ERROR_WORDS_NOT_PROVIDED;
    }

    for (const auto& word : words)
    {
        for (const auto& file : this->findFiles(word))
        {
            out_Paths->insert(file);
        }
    }

    return 0;
}

int FileIndexer::getAllIndexedEntries(std::set<std::string>* out_paths) const
{
    if (this->isCurrentlyIndexing.load())
    {
        return ERROR_CURRENTLY_INDEXING;
    }
    *out_paths = this->allIndexedPaths;
    return 0;
}
