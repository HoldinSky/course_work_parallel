#include "text_utils/fileParser.h"

#include <cstdint>
#include <iostream>

// 1 Megabyte
#define BUFFER_SIZE 1048576

Range findWord(char const* const str, int64_t const& length, int64_t const& offset, bool (*isDelimiter)(char const&))
{
    int64_t wordStart = offset, wordEnd = offset;
    while (wordStart < length && isDelimiter(str[wordStart]))
    {
        ++wordStart;
    }
    if (wordStart >= length)
    {
        wordStart = length - 1;
    }

    wordEnd = wordStart + 1;
    while (wordEnd < length - 1 && !isDelimiter(str[wordEnd + 1]))
    {
        ++wordEnd;
    }

    return Range{wordStart, wordEnd};
}

void parseInputStreamByWord(std::istream& inputData, std::function<void(char const*)> const& actionPerWord,
                            bool (*isDelimiter)(char const&))
{
    char buffer[BUFFER_SIZE]{};
    int64_t charsProcessed = 0;
    std::string word;
    std::string prevWordPart;

    Range range{};
    while (inputData)
    {
        range = {-1, -1};

        inputData.seekg(charsProcessed, std::ios::beg);
        inputData.read(buffer, BUFFER_SIZE - 1);
        buffer[BUFFER_SIZE - 1] = 0;
        int64_t readBytes = inputData.gcount();

        while (readBytes > range.end + 1)
        {
            range = findWord(buffer, readBytes, range.end + 1, isDelimiter);
            word = std::string(buffer + range.start, range.length());

            if (range.length() == readBytes)
            {
                prevWordPart += word;
                break;
            }
            else if (range.end >= readBytes - 1)
            {
                if (!prevWordPart.empty())
                {
                    actionPerWord(prevWordPart.c_str());
                }
                prevWordPart = word;
                break;
            }

            word = prevWordPart.append(word);
            actionPerWord(word.c_str());
            prevWordPart = "";
        }
        charsProcessed += range.end + 1;
    }

    if (!prevWordPart.empty())
    {
        actionPerWord(prevWordPart.c_str());
    }
}
