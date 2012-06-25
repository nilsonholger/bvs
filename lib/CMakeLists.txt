cmake_minimum_required(VERSION 2.8.6)

include(cmake/toolbox.cmake)

project(LIBBVS)

include_directories(include)
include_directories(src)
add_subdir_lib(src bvs SHARED BVS.cc BVSConfig.cc BVSConnector.cc BVSLogger.cc BVSLogSystem.cc BVSMaster.cc BVSModule.cc)
target_link_libraries(bvs dl pthread)

add_custom_target(doc doxygen)
