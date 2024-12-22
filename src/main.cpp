#include <iostream>
#include <fstream>

#include "FileIndexer.h"


const std::string data_root_location = R"(D:\prg\cpp\inverted_index\data)";
const std::string test_path = data_root_location + R"(\test_neg\2251_2.txt)";

void actionPerWord(char const* word) {
    std::cout << word << std::endl;
}

int main() {
    FileIndexer indexer;

    indexer.indexDirectory(data_root_location);
    for (const auto files = indexer.findFiles("I"); const auto& filePath : files) {
        std::cout << filePath << std::endl;
    }

    return 0;
}
