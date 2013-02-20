cmake_minimum_required(VERSION 2.8.6)

project(BVSD)
include(../lib/cmake/toolbox.cmake)

add_subdir_exec(src bvsd bvsd.cc)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath,.")
target_link_libraries(bvsd bvs)
