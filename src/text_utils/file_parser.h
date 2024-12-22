#ifndef CW_FILE_PARSER_H
#define CW_FILE_PARSER_H

#include <functional>
#include <istream>

void parseInputStreamByWord(std::istream &inputData, const std::function<void(char const *)> &actionPerWord);

#endif // CW_FILE_PARSER_H
