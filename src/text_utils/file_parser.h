#ifndef CW_TEXT_UTILS_FILE_PARSER_H
#define CW_TEXT_UTILS_FILE_PARSER_H

#include "common.h"

#include <functional>
#include <set>

const std::set delimiters = {
    '\0', '\t', '\n', '\r',
    ' ', '\'', '`', '"',
    ',', '.', '!', '?', '(', ')',
    '/', ';', ':', '<', '>'
};

inline bool defaultIsDelimiter(char const& ch)
{
    return delimiters.contains(ch);
}

inline bool isNotAlpha(char const& ch)
{
    return !isalpha(ch);
}

void parseInputStreamByWord(std::istream& iStream,
                            std::function<void(char const*)> const& actionPerWord,
                            std::function<bool(char const&)> const& isDelimiter = isNotAlpha);

#endif // CW_TEXT_UTILS_FILE_PARSER_H
