#include <fstream>

#include "FileIndexer.h"

const std::string data_root_location = R"(D:\prg\cpp\inverted_index\data)";

int main() {
    FileIndexer indexer(false);

    const auto result = indexer.all({"I", "am", "sorry"});

    return 0;
}
