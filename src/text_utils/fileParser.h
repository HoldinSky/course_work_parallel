#ifndef CW_TEXT_UTILS_FILE_PARSER_H
#define CW_TEXT_UTILS_FILE_PARSER_H

#include <functional>
#include <istream>

void parseInputStreamByWord(std::istream &inputData, std::function<void(char const *)> const &actionPerWord);
void parseInputStreamByWord(std::istream &inputData, char const& delimiter, std::function<void(char const *)> const &actionPerWord);

#endif // CW_TEXT_UTILS_FILE_PARSER_H
