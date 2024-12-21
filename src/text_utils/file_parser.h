#ifndef CW_FILE_PARSER_H
#define CW_FILE_PARSER_H

#include <istream>

void parseInputStreamByWord(std::istream &inputData, void (*actionPerWord)(char const *));

#endif // CW_FILE_PARSER_H
