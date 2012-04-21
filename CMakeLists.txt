cmake_minimum_required(VERSION 2.8.6)

project(BVSBASE)
set(CMAKE_BUILD_TYPE "DEBUG")
include(libbvs/cmake/toolbox.cmake)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

include_directories(libbvs/include)
add_subdirectory(libbvs)
add_subdirectory(bvsd)
add_subdirectory(modules)
