cmake_minimum_required(VERSION 3.29)
project(inverted_index)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/)

add_executable(
        inverted_index src/main.cpp
        src/text_utils/file_parser.h src/text_utils/file_parser.cpp
        src/FileIndexer.h src/FileIndexer.cpp
        src/thread_pool/pool.h src/thread_pool/pool.cpp
        src/main_helper.h
        src/api/server.h
        src/api/server.cpp
        src/common.h
        src/common.cpp
)
