#include <fstream>

#include "FileIndexer.h"

const std::string data_root_location = R"(D:\prg\cpp\inverted_index\data)";

int main()
{
    FileIndexer indexer(false);

    std::set<std::string> result;
    indexer.all({"I", "am", "sorry"}, &result);

    for (auto const& item : result)
    {
        printf("%s\n", item.c_str());
    }

    return 0;
}
