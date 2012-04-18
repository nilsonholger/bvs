project(LIBBVS)
cmake_minimum_required(VERSION 2.8.6)
include(cmake/toolbox.cmake)

include_directories(include)
include_directories(src)
add_subdir_lib(src bvs SHARED BVS.cc BVSConfig.cc BVSLogger.cc BVSLogSystem.cc BVSModule.cc BVSModuleLoader.cc)
target_link_libraries(bvs dl)
