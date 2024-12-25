#include "text_utils/fileParser.h"

#include <set>
#include <cstdint>
#include <iostream>

// 1 Megabyte
#define BUFFER_SIZE 1048576

const std::set chars_to_skip = {
    '\0', '\t', '\n', '\r',
    ' ', '\'', '`', '"',
    ',', '.', '!', '?', '(', ')',
    '/',
};

inline bool isvalid(char const &c) {
    return !chars_to_skip.contains(c);
}

inline bool isvalid2(char const &c) {
    return isalpha(c);
}

int64_t findWordsAndPerformAction(char const *const str, int64_t const length, const std::function<void(char const *)>& action) {
    int64_t wordStart = 0, wordEnd = 0;
    while (true) {
        while (wordStart < length && !isvalid2(str[wordStart])) {
            wordStart++;
        }
        if (wordStart >= length) {
            return wordEnd;
        }
        wordEnd = wordStart;
        while (wordEnd < length && isvalid2(str[wordEnd])) {
            wordEnd++;
        }
        if (wordEnd >= length && isvalid2(str[wordEnd - 1])) {
            return wordStart;
        }
        std::string word(str + wordStart, wordEnd - wordStart);
        action(word.c_str());
        wordStart = wordEnd;
    }
}

void parseInputStreamByWord(std::istream &inputData, const std::function<void(char const *)>& actionPerWord) {
    char buffer[BUFFER_SIZE]{};
    int64_t charsProcessed = 0, effectiveChars = 0;

    while (inputData) {
        inputData.seekg(charsProcessed, std::ios::beg);
        inputData.read(buffer, BUFFER_SIZE - 1);
        buffer[BUFFER_SIZE - 1] = 0;

        effectiveChars = findWordsAndPerformAction(buffer, inputData.gcount(), actionPerWord);
        charsProcessed += effectiveChars;
    }
}
