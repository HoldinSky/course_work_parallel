#include "http_specific.h"

#include <cstring>
#include <sstream>
#include <fstream>

void parseRequest(const char* requestBuffer, HttpRequest* out_request, HttpResponse* out_response)
{
    std::vector<std::string> headers;
    std::string body;

    std::istringstream stream(requestBuffer);
    std::string line;

    if (std::getline(stream, line))
    {
        out_request->topLine = trim(line);
    }

    if (strncmp(requestBuffer, MethodStrings::GET, MethodStrings::GET_LEN) != 0)
    {
        out_response->error = "Failed to recognize request method";
        out_response->topLine = TOP_LINE_BAD_REQUEST;
        return;
    }

    while (std::getline(stream, line) && !line.empty() && line != "\r")
    {
        auto header_line = trim(line);

        auto colon_ind = header_line.find_first_of(':');
        if (colon_ind == std::string::npos) continue;

        auto header = trim(header_line.substr(0, colon_ind));
        auto value = trim(header_line.substr(colon_ind + 1));

        out_request->headers[header] = value;
    }

    std::stringstream bodyStream;
    while (std::getline(stream, line))
    {
        bodyStream << line << "\n";
    }
    out_request->body = bodyStream.str();

    if (!out_request->body.empty() && out_request->body.back() == '\n')
    {
        out_request->body.pop_back();
    }
}

void parseHttpTopLine(const std::string& input, HttpTopLine* out_line, HttpResponse* out_response)
{
    auto words = split(input, ' ');

    if (words.size() != 3)
    {
        out_response->error = "Bad request line format";
        out_response->topLine = TOP_LINE_BAD_REQUEST;
        return;
    }

    if (strcmp(words[0].c_str(), MethodStrings::GET) != 0)
    {
        out_response->error = "Failed to recognize request method";
        out_response->topLine = TOP_LINE_BAD_REQUEST;
        return;
    }

    out_line->method = Method::GET;
    out_line->requestPath = words[1];
    out_line->protocol = words[2];
}

std::string generateResponseHeaders(const std::string& body)
{
    std::string headers = "Content-Length: " + std::to_string(body.length()) + "\r\n";

    headers += "Content-Type: text/html; charset=utf-8\r\n";

    return headers;
}

std::string composeResponse(HttpRequest const& request, HttpResponse const& response,
                            std::set<std::string> const& paths)
{
    auto const headers = generateResponseHeaders(response.body);

    return request.topLine + headers + "\r\n" + response.body;
}
