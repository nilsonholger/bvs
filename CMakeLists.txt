cmake_minimum_required(VERSION 2.8.6)
include(libbvs/cmake/toolbox.cmake)
set(CMAKE_BUILD_TYPE "DEBUG")

if(NOT CMAKE_CXX_FLAGS)
 set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -Weffc++ -pedantic -pedantic-errors -Winline")
endif(NOT CMAKE_CXX_FLAGS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

if(GPROF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif(GPROF)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

project(BVSBASE)

include_directories(libbvs/include)
add_subdirectory(libbvs)
add_subdirectory(bvsd)
add_subdirectory(modules)
