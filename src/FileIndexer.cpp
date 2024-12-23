#include "FileIndexer.h"
#include "text_utils/file_parser.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

// utilities

void FileIndexer::addMapping(const std::string &word, const std::string &path) {
    std::set<std::string> pathSet;
    try {
        pathSet = this->index.at(word);
        pathSet.insert(path);
    } catch (const std::out_of_range &_) {
        pathSet.insert(path);
    }

    this->index.insert_or_assign(word, pathSet);
}

void FileIndexer::addMapping(const std::string &word, const std::set<std::string> &paths) {
    std::set<std::string> pathSet = paths;
    try {
        pathSet = this->index.at(word);
        for (const auto &path: paths) {
            pathSet.insert(path);
        }
    } catch (const std::out_of_range &_) {
    }

    this->index.insert_or_assign(word, pathSet);
}

// private here

int FileIndexer::indexFile(const std::string &path) {
    std::ifstream file(path, std::ios::in);
    if (!file) {
        fprintf(stderr, "[ERR] Could not open file %s\n", path.c_str());
        return -1;
    }

    parseInputStreamByWord(file, [&](char const *word) { this->indexWord(word, path); });

    return 0;
}

void FileIndexer::indexWord(const char *word, const std::string &path) {
    const auto wordStr = std::string(word);
    this->addMapping(wordStr, path);
}

std::set<std::string> FileIndexer::findFiles(const std::string &word) {
    try {
        return this->index.at(word);
    } catch (const std::out_of_range &_) {
        return {};
    }
}

void FileIndexer::readIndexFromFile() {
    if (!fs::exists(defaultIndexFile)) {
        return;
    }

    std::ifstream file(defaultIndexFile, std::ios::in);
    if (!file || !file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::set<std::string> storedPaths;

        std::getline(ss, cell, ',');
        const auto word = cell;

        while (std::getline(ss, cell, ',')) {
            storedPaths.insert(cell);
        }

        this->addMapping(word, storedPaths);
    }
}

void FileIndexer::saveIndexToFile() {
    const bool exists = fs::exists(defaultIndexFile);
    if (!this->overwriteSave && exists) {
        return;
    }

    if (!exists) {
        fs::path targetFilePath(defaultIndexFile);

        if (targetFilePath.has_parent_path()) {
            fs::create_directories(targetFilePath.parent_path());
        }
    }

    std::ofstream file(defaultIndexFile, std::ios::out);
    if (!file || !file.is_open()) {
        return;
    }

    std::string line;
    std::stringstream ss(line);
    for (const auto &[word, paths]: this->index) {
        ss << word;
        for (const auto &path: paths) {
            ss << ',' << path;
        }
        ss << '\n';
        file << ss.str();
        ss.str(std::string());
    }
}

// public here

int FileIndexer::indexDirectory(const std::string &path) {
    if (!fs::exists(path)) {
        fprintf(stderr, "[ERR] Directory %s does not exist\n", path.c_str());
        return -1;
    }

    if (!fs::is_directory(path)) {
        fprintf(stderr, "[ERR] %s is not a directory\n", path.c_str());
        return -1;
    }

    for (const auto &entry: fs::directory_iterator(path)) {
        if (entry.is_directory()) {
            this->indexDirectory(entry.path().string());
        } else {
            this->indexFile(entry.path().string());
        }
    }

    return 0;
}

std::set<std::string> FileIndexer::all(const std::vector<std::string> &words) {
    if (words.empty()) {
        return {};
    }
    std::set<std::string> result = this->findFiles(words[0]);
    std::set<std::string> intersection;

    for (uint64_t i = 1; i < words.size(); ++i) {
        const auto &set = this->findFiles(words[i]);

        std::set_intersection(
            result.begin(), result.end(),
            set.begin(), set.end(),
            std::inserter(intersection, intersection.begin())
        );
        result = intersection;
        intersection.clear();
    }

    return result;
}

std::set<std::string> FileIndexer::any(const std::vector<std::string> &words) {
    if (words.empty()) {
        return {};
    }
    std::set<std::string> result;

    for (const auto &word: words) {
        for (const auto &file: this->findFiles(word)) {
            result.insert(file);
        }
    }

    return result;
}
