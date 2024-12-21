#include <iostream>
#include <fstream>

#include "text_utils/file_parser.h"

const std::string data_root_location = R"(D:\prg\cpp\inverted_index\data)";
const std::string test_path = data_root_location + R"(\test_neg\2251_2.txt)";

void actionPerWord(char const* word) {
    std::cout << word << std::endl;
}

int main() {
    std::ifstream fileInput(test_path, std::ios::in | std::ios::binary);
    if (!fileInput) {
        std::cerr << "Failed to open file: " << test_path << std::endl;
        return 1;
    }

    std::istream& inputData = fileInput;

    parseInputStreamByWord(inputData, actionPerWord);
    return 0;
}
