#ifndef CW_TEXT_UTILS_FILE_PARSER_H
#define CW_TEXT_UTILS_FILE_PARSER_H

#include "common.h"

#include <cstdint>
#include <functional>
#include <set>

const std::set delimitersCharSet = {
    '\0', '\t', '\n', '\r',
    ' ', '\'', '`', '"',
    ',', '.', '!', '?', '(', ')',
    '/',
};

inline bool defaultIsDelimiter(char const& ch)
{
    return delimitersCharSet.contains(ch);
}

void parseInputStreamByWord(std::istream& inputData, std::function<void(char const*)> const& actionPerWord,
                            bool (*isDelimiter)(char const&) = defaultIsDelimiter);

/// @param str where to look for the word
/// @param length total length of str
/// @param offset index from which to look for the word
/// @param isDelimiter function to distinguish words from delimiters between them
/// @return Range indices of the next word, both inclusive. If all characters after offset are delimiters [length - 1, length - 1] is returned.
Range findWord(char const* const str, int64_t const& length,
               int64_t const& offset, bool (*isDelimiter)(char const&) = defaultIsDelimiter);

#endif // CW_TEXT_UTILS_FILE_PARSER_H
