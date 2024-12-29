#include "file_parser.h"

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
    constexpr int size = BYTES_IN_1KB * 4;
    char buffer[size]{};
    int64_t charsProcessed = 0, effectiveChars = 0;

    while (iStream)
    {
        iStream.seekg(charsProcessed, std::ios::beg);
        iStream.read(buffer, size - 1);
        buffer[size - 1] = 0;

        effectiveChars = findWordsAndPerformAction(buffer, iStream.gcount(), actionPerWord, isDelimiter);
        charsProcessed += effectiveChars;
    }
}
