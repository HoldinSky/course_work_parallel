#include "FileIndexer.h"
#include "text_utils/file_parser.h"

#include <filesystem>
#include <fstream>
#include <iostream>

void FileIndexer::indexWord(const char *word, const std::string &path) {
    const auto wordStr = std::string(word);
    std::set<std::string> pathSet;
    try {
        pathSet = this->index.at(wordStr);
        pathSet.insert(path);
    } catch (const std::out_of_range &_) {
        pathSet.insert(path);
    }

    this->index.insert_or_assign(wordStr, pathSet);
}

int FileIndexer::indexFile(const std::string &path) {
    std::ifstream file(path, std::ios::in);
    if (!file) {
        std::cerr << "Could not open file " << path << std::endl;
        return -1;
    }

    parseInputStreamByWord(file, [&](char const *word) { this->indexWord(word, path); });

    return 0;
}

int FileIndexer::indexDirectory(const std::string &path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Directory " << path << " does not exist\n";
        return -1;
    }

    if (!std::filesystem::is_directory(path)) {
        std::cerr << path << " is not a directory\n";
        return -1;
    }

    for (const auto &entry: std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            this->indexDirectory(entry.path().string());
        } else {
            this->indexFile(entry.path().string());
        }
    }

    return 0;
}

std::set<std::string> FileIndexer::findFiles(const std::string &word) {
    try {
        return this->index.at(word);
    } catch (const std::out_of_range &e) {
        return {};
    }
}
