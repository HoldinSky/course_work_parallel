#include <cstdint>
#include <iostream>

#include "text_utils/file_parser.h"
#include "common.h"

int64_t findWordsAndPerformAction(char const* const str, int64_t const length,
                                  std::function<void(char const*)> const& action,
                                  std::function<bool(char const&)> const& isDelimiter)
{
    int64_t wordStart = 0, wordEnd = 0;
    while (true)
    {
        while (wordStart < length && isDelimiter(str[wordStart]))
        {
            wordStart++;
        }
        if (wordStart >= length)
        {
            return wordEnd;
        }
        wordEnd = wordStart;
        while (wordEnd < length && !isDelimiter(str[wordEnd]))
        {
            wordEnd++;
        }
        if (wordEnd >= length && !isDelimiter(str[wordEnd - 1]))
        {
            return wordStart;
        }
        std::string word(str + wordStart, wordEnd - wordStart);
        action(word.c_str());
        wordStart = wordEnd;
    }
}

void parseInputStreamByWord(std::istream& iStream,
                            std::function<void(char const*)> const& actionPerWord,
                            std::function<bool(char const&)> const& isDelimiter)
{
    char buffer[BYTES_IN_1MB]{};
    int64_t charsProcessed = 0, effectiveChars = 0;

    while (iStream)
    {
        iStream.seekg(charsProcessed, std::ios::beg);
        iStream.read(buffer, BYTES_IN_1MB - 1);
        buffer[BYTES_IN_1MB - 1] = 0;

        effectiveChars = findWordsAndPerformAction(buffer, iStream.gcount(), actionPerWord, isDelimiter);
        charsProcessed += effectiveChars;
    }
}
