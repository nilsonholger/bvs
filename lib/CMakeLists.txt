cmake_minimum_required(VERSION 2.8.6)

include(cmake/toolbox.cmake)

project(LIBBVS)

include_directories(include)
include_directories(src)
add_subdir_lib(src bvs SHARED bvs.cc config.cc connector.cc control.cc loader.cc logger.cc logsystem.cc module.cc)
target_link_libraries(bvs dl pthread)

add_custom_target(doc doxygen)
