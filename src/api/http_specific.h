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
    UNKNOWN = 0,
};

struct MethodStrings
{
    static constexpr char const* const GET = "GET";
    static constexpr uint32_t GET_LEN = strLength(MethodStrings::GET);
};

struct HttpRequest
{
    std::string topLine{};
    std::unordered_map<std::string, std::string> headers{};
    std::string body{};
};

struct HttpTopLine
{
    Method method = UNKNOWN;
    std::string requestPath{};
    std::string protocol{};
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
    static constexpr const char* const filesWithAllWords = "/file-all-words";
    static constexpr const char* const reindex = "/reindex";
};

void parseRequest(const char* requestBuffer, HttpRequest* out_request, HttpResponse* out_response);

void parseHttpTopLine(const std::string& input, HttpTopLine* out_line, HttpResponse* out_response);

std::string composeResponse(HttpRequest const& request, HttpResponse const& response,
                            std::set<std::string> const& paths = {});

#endif //CW_HTTP_SPECIFIC_H
