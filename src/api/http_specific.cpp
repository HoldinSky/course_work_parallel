#include "http_specific.h"

#include <cstring>
#include <sstream>
#include <fstream>

int parseMethod(std::string const& methodStr, HttpRequest* out_request)
{
    if (strcmp(methodStr.c_str(), MethodStrings::GET) == 0)
    {
        out_request->method = Method::GET;
        return 0;
    }
    else if (strcmp(methodStr.c_str(), MethodStrings::POST) == 0)
    {
        out_request->method = Method::POST;
        return 0;
    }
    out_request->method = Method::UNKNOWN;

    return -1;
}

int parseHttpTopLine(HttpRequest* out_request, HttpResponse* out_response)
{
    auto topLineWords = split(out_request->topLine, ' ');

    if (topLineWords.size() != 3)
    {
        out_response->error = "Bad request line format";
        out_response->topLine = TOP_LINE_BAD_REQUEST;
        return -1;
    }

    if (parseMethod(topLineWords[0], out_request))
    {
        out_response->error = "Failed to recognize request method";
        out_response->topLine = TOP_LINE_BAD_REQUEST;
        return -1;
    }

    out_request->path = topLineWords[1];
    out_request->protocol = topLineWords[2];
    return 0;
}

void parseHeaders(std::istream& requestStream, HttpRequest* out_request)
{
    std::string line;
    while (std::getline(requestStream, line) && !line.empty() && line != "\r")
    {
        auto header_line = trim(line);

        auto colon_ind = header_line.find_first_of(':');
        if (colon_ind == std::string::npos) continue;

        auto header = trim(header_line.substr(0, colon_ind));
        auto value = trim(header_line.substr(colon_ind + 1));

        out_request->headers[header] = value;
    }
}

void parseBody(std::istream& requestStream, HttpRequest* out_request)
{
    std::string line;
    std::stringstream bodyStream;
    while (std::getline(requestStream, line))
    {
        bodyStream << line << "\n";
    }

    out_request->body = bodyStream.str();
    if (!out_request->body.empty() && out_request->body.back() == '\n')
    {
        out_request->body.pop_back();
    }
}

void parseRequest(const char* requestBuffer, HttpRequest* out_request, HttpResponse* out_response)
{
    std::istringstream stream(requestBuffer);

    if (std::string line; std::getline(stream, line))
    {
        out_request->topLine = trim(line);
    }

    if (parseHttpTopLine(out_request, out_response))
    {
        return;
    }

    parseHeaders(stream, out_request);
    parseBody(stream, out_request);
}

std::string generateResponseHeaders(HttpResponse const& resp)
{
    std::string headers = "Content-Length: " + std::to_string(resp.body.length() + resp.error.length()) + "\r\n";

    headers += "Content-Type: text/plain; charset=utf-8\r\n";
    headers += "Access-Control-Allow-Origin: *\r\n";

    return headers;
}

std::string composeResponse(HttpRequest const& request, HttpResponse const& response)
{
    auto const headers = generateResponseHeaders(response);

    std::stringstream outStream;
    outStream << response.topLine;
    outStream << headers << "\r\n";
    if (!response.body.empty())
    {
        outStream << response.body <<"\r\n";
    }
    if (!response.error.empty())
    {
        outStream << response.error << "\r\n";
    }

    return outStream.str();
}
