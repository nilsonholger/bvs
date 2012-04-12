include(toolbox.cmake)
project(LIBBVS)

include_directories(include)
include_directories(src)
add_subdir_lib(src bvs SHARED BVS.cc BVSConfig.cc BVSLogger.cc BVSLogSystem.cc BVSModule.cc BVSModuleLoader.cc)
