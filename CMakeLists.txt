cmake_minimum_required(VERSION 3.29)
project(inverted_index)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/)

add_executable(
        inverted_index src/main.cpp
        src/common.h
        src/thread_pool/pool.h src/thread_pool/pool.cpp
        src/file_indexer.h src/file_indexer.cpp
        src/text_utils/file_parser.h src/text_utils/file_parser.cpp
        src/api/socket_utils.h src/api/socket_utils.cpp
        src/api/route_handler.h src/api/route_handler.cpp
        src/api/http_specific.h src/api/http_specific.cpp
        src/api/server.h src/api/server.cpp
)

if (WIN32)
    target_link_libraries(inverted_index ws2_32)
endif ()