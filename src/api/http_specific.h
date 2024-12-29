#ifndef CW_HTTP_SPECIFIC_H
#define CW_HTTP_SPECIFIC_H

#include <string>
#include <unordered_map>

#include "socket_utils.h"

#define TOP_LINE_200 "HTTP/1.1 200 OK\r\n"
#define TOP_LINE_NOT_FOUND "HTTP/1.1 404 Not Found\r\n"
#define TOP_LINE_BAD_REQUEST "HTTP/1.1 400 Bad Request\r\n"
#define TOP_LINE_INTERNAL_ERROR "HTTP/1.1 500 Internal Server Error\r\n"

enum Method
{
    GET = 1,
    POST = 2,
    UNKNOWN = 0,
};

struct MethodStrings
{
    static constexpr char const* const GET = "GET";
    static constexpr uint32_t GET_LEN = strLength(MethodStrings::GET);

    static constexpr char const* const POST = "POST";
    static constexpr uint32_t POST_LEN = strLength(MethodStrings::POST);
};

struct HttpRequest
{
    std::string topLine{};

    Method method = UNKNOWN;
    std::string path{};
    std::string protocol{};

    std::unordered_map<std::string, std::string> headers{};
    std::string body{};
};

struct HttpResponse
{
    std::string topLine{};
    std::unordered_map<std::string, std::string> headers{};
    std::string body{};

    std::string error{};
};

struct RequestPath
{
    static constexpr const char* const addToIndex = "/add-to-index";
    static constexpr const char* const removeFromIndex = "/remove-from-index";
    static constexpr const char* const filesWithAnyWord = "/files-any-word";
    static constexpr const char* const filesWithAllWords = "/files-all-words";
    static constexpr const char* const reindex = "/reindex";
    static constexpr const char* const getAllIndexed = "/get-all-indexed";
};

void parseRequest(const char* requestBuffer, HttpRequest* out_request, HttpResponse* out_response);

std::string composeResponse(HttpRequest const& request, HttpResponse const& response);

inline std::string methodToString(Method const& method)
{
    switch (method)
    {
        case Method::GET:
            return "GET";
        case Method::POST:
            return "POST";
        default:
            return "UNKNOWN";
    }
}

#endif //CW_HTTP_SPECIFIC_H
