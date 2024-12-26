#include "FileIndexer.h"
#include "text_utils/fileParser.h"

#include <string>
#include <fstream>

std::string const dataRootPath = R"(D:\prg\cpp\inverted_index\data)";

inline void actionPerWord(char const* word)
{
    printf("%s\n", word);
}

void test()
{
    std::ifstream testFile(dataRootPath + "\\test.txt");
    parseInputStreamByWord(testFile, actionPerWord);
}

void app()
{
    FileIndexer indexer(false);
    std::set<std::string> result;
    indexer.all({"I", "am", "sorry"}, &result);

    for (auto const& item : result)
    {
        printf("%s\n", item.c_str());
    }
}

int main()
{
    app();

    return 0;
}
