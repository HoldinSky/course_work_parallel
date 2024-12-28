#include "text_utils/file_parser.h"

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
                            uint32_t count, bool (*isDelimiter)(char const&))
{
    bool const ignoreCount = count == 0;
    char buffer[BUFFER_SIZE]{};
    int64_t charsProcessed = 0;
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
            auto word = std::string(buffer + range.start, range.length());

            if (range.length() == readBytes)
            {
                prevWordPart += word;
                break;
            }

            if (range.end >= readBytes - 1)
            {
                if (!prevWordPart.empty())
                {
                    actionPerWord(prevWordPart.c_str());
                    if (!ignoreCount && --count <= 0)
                    {
                        return;
                    }
                }
                prevWordPart = word;
                break;
            }

            word = prevWordPart.append(word);
            actionPerWord(word.c_str());
            if (!ignoreCount && --count <= 0)
            {
                return;
            }
            prevWordPart = "";
        }
        charsProcessed += range.end + 1;
    }

    if (!prevWordPart.empty())
    {
        actionPerWord(prevWordPart.c_str());
    }
}
