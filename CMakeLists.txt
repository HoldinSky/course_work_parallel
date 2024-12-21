cmake_minimum_required(VERSION 3.29)
project(inverted_index)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/)

add_executable(
        inverted_index src/main.cpp
        src/text_utils/file_parser.h src/text_utils/file_parser.cpp
        src/FileIndexer.cpp
        src/FileIndexer.h
)
