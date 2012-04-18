cmake_minimum_required(VERSION 2.8.6)

option(USE_CLANG "build using clang" OFF)
if(USE_CLANG)
    SET (CMAKE_C_COMPILER             "clang")
    SET (CMAKE_C_FLAGS                "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_CXX_COMPILER             "clang++")
    SET (CMAKE_CXX_FLAGS                "-Wall -std=c++11 -Wc++11-extensions")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
endif(USE_CLANG)

