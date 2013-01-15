cmake_minimum_required(VERSION 2.8.6)

project(BVSD)
include(../lib/cmake/toolbox.cmake)

add_subdir_exec(src bvsd bvsd.cc)
target_link_libraries(bvsd bvs)
