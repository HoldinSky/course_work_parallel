#include <iostream>
#include <fstream>

#include "FileIndexer.h"

const std::string data_root_location = R"(D:\prg\cpp\inverted_index\data)";

int main() {
    ThreadPool pool;
    FileIndexer indexer;

    const auto elapsedMillis = measureExecutionTime([&] {
        indexer.indexDirectory(data_root_location);
    });

    printf("Completed in %lld ms\n", elapsedMillis.count());

    return 0;
}
