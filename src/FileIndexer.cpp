#include "FileIndexer.h"
#include "text_utils/file_parser.h"

#include <filesystem>
#include <fstream>
#include <iostream>

int FileIndexer::indexFile(const std::string &path) {
    std::ifstream file(path, std::ios::in);
    if (!file) {
        std::cerr << "Could not open file " << path << std::endl;
        return -1;
    }

    auto addFile = [](char const * word) {
        
    };

    parseInputStreamByWord(static_cast<std::istream&>(file), addFile);
}


int FileIndexer::indexDirectory(const std::string &path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "Directory does not exist" << std::endl;
        return -1;
    }

    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        this->indexDirectory(entry.path().string());
    }
}
