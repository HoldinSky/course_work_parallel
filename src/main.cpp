#include <string>
#include <fstream>

#include "file_indexer.h"
#include "text_utils/file_parser.h"
#include "api/server.h"

std::string const dataRootPath = R"(D:\prg\cpp\inverted_index\data)";

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        fprintf(stderr, "WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    srv::serverRoutine();

    return 0;
}
