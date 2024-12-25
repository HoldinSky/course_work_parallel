cmake_minimum_required(VERSION 3.29)
project(inverted_index)

set(CMAKE_CXX_STANDARD 20)

include_directories(src/)

add_executable(
        inverted_index src/main.cpp
        src/common.h
        src/text_utils/fileParser.h src/text_utils/fileParser.cpp
        src/thread_pool/pool.h src/thread_pool/pool.cpp
        src/FileIndexer.h src/FileIndexer.cpp
        src/api/server.h
        src/api/server.cpp
        src/api/socketUtils.h src/api/socketUtils.cpp
        src/api/router.h
        src/api/router.cpp
)
