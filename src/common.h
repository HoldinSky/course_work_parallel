#ifndef CW_COMMON_H
#define CW_COMMON_H

#include <mutex>
#include <shared_mutex>
#include <condition_variable>

#include <cstdint>
#include <chrono>

#define ERROR_FILE_CANNOT_BE_OPENED 270

#define ERROR_CURRENTLY_INDEXING 271
#define ERROR_PATH_DOES_NOT_EXIST 272
#define ERROR_PATH_ALREADY_INDEXED 273
#define ERROR_PATH_ALREADY_NOT_INDEXED 274
#define ERROR_WORDS_NOT_PROVIDED 275
#define ERROR_PATHS_NOT_PROVIDED 276
#define ERROR_EMPTY_BODY 277

#define BYTES_IN_1MB 1048576
#define BYTES_IN_1KB 1024

using rwLock = std::shared_mutex;
using sharedLock = std::shared_lock<rwLock>;
using exclusiveLock = std::unique_lock<rwLock>;

static rwLock stdoutLock;
static rwLock commonMutex;

static std::condition_variable_any commonMonitor;

constexpr uint32_t strLength(const char* const str)
{
    uint32_t len = 0;

    for (; str[len] != '\0'; len++)
    {
    }

    return len;
}

inline std::string trim(const std::string& s)
{
    auto const start = s.find_first_not_of(" \t\r\n");
    auto const end = s.find_last_not_of(" \t\r\n");
    return start == std::string::npos ? "" : s.substr(start, end - start + 1);
}

inline std::vector<std::string> split(const std::string& str, char const& delimiter)
{
    std::vector<std::string> output;
    std::string word;
    std::istringstream stream(str);

    while (std::getline(stream, word, delimiter))
    {
        output.push_back(word);
    }

    return output;
}

inline std::string MapErrorCodeToString(int32_t const& code)
{
    switch (code)
    {
    case ERROR_CURRENTLY_INDEXING: return "Indexing is in progress";
    case ERROR_PATH_DOES_NOT_EXIST: return "Path does not exist";
    case ERROR_PATH_ALREADY_INDEXED: return "Path is already included in index";
    case ERROR_PATH_ALREADY_NOT_INDEXED: return "Path is already excluded from index";
    case ERROR_WORDS_NOT_PROVIDED: return "Words were not provided";
    case ERROR_PATHS_NOT_PROVIDED: return "Paths were not provided";
    case ERROR_FILE_CANNOT_BE_OPENED: return "File cannot be opened";
    case ERROR_EMPTY_BODY: return "No data was provided (empty body)";
    default: return "Something went wrong";
    }
}

/// both indices are inclusive
struct Range
{
    int64_t start{-1};
    int64_t end{-1};

    [[nodiscard]] int64_t length() const
    {
        return end - start + 1;
    }
};

template<typename FT>
std::chrono::duration<int64_t, std::milli>
measureTimeMillis(FT func) {
    const auto start = std::chrono::high_resolution_clock::now();
    func();
    const auto end = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

#endif // CW_COMMON_H
