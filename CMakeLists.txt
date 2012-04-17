cmake_minimum_required(VERSION 2.8.6)
include(libbvs/cmake/compiler.cmake)
include(libbvs/cmake/macros.cmake)
set(CMAKE_BUILD_TYPE "DEBUG")

if(NOT CMAKE_CXX_FLAGS)
 set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Weffc++ -pedantic -pedantic-errors -ansi -Winline")
endif(NOT CMAKE_CXX_FLAGS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

if(APPLE)
    set(CMAKE_CXX_COMPILER "/usr/local/bin/c++-4.7")
endif(APPLE)

if(GPROF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif(GPROF)

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

project(BVSBASE)

add_custom_target(run ${CMAKE_SOURCE_DIR}/run.sh)

include_directories(libbvs/include)
add_subdirectory(libbvs)
add_subdirectory(bvsd)
add_subdirectory(modules)
