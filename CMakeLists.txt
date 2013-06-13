cmake_minimum_required(VERSION 2.8.6)

project(BVS)
set(CMAKE_BUILD_TYPE "DEBUG")
include(lib/cmake/toolbox.cmake)

# general settings
include_directories(lib/include)

set(CMAKE_SKIP_RPATH ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath,.")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# output directory
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# symlink run script
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/run ${CMAKE_BINARY_DIR}/run)
execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/run ${CMAKE_BINARY_DIR}/bin/run)

# documentation target
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating documentation for everything!" VERBATIM)
else()
	add_custom_target(doc echo "DOXYGEN NOT FOUND: it is required to build the documentation! Please consider installing it.")
endif()


# BVS_LOG_SYSTEM
set(BVS_LOG_SYSTEM ON CACHE BOOL "Enable BVS builtin log system.")
if(BVS_LOG_SYSTEM)
	add_definitions(-DBVS_LOG_SYSTEM)
endif()

# BVS_THREAD_NAMES
set(BVS_THREAD_NAMES ${UNIX} CACHE BOOL "Enable thread naming (htop: enable 'Show custom thread names', or try 'ps -La'). UNIX ONLY!")
mark_as_advanced(BVS_THREAD_NAMES)
if(BVS_THREAD_NAMES)
	add_definitions(-DBVS_THREAD_NAMES)
endif()

# BVS_STATIC_MODULES
set(BVS_STATIC_MODULES OFF CACHE BOOL "Enable compilation of static modules and creation of single library object!")
mark_as_advanced(BVS_STATIC_MODULES)
if(BVS_STATIC_MODULES)
	set(BVS_MODULE_TYPE STATIC)
	set(BVS_LINK_LIBRARIES "")
	add_definitions(-DBVS_STATIC_MODULES)
else()
	set(BVS_MODULE_TYPE SHARED)
	set(BVS_LINK_LIBRARIES bvs)
endif()


# ANDROID
set(BVS_ANDROID_APP FALSE CACHE BOOL "Generate Android targets!")
mark_as_advanced(BVS_ANDROID_APP)
if(BVS_ANDROID_APP)
	include_directories(android)
	add_subdirectory(android)
#TODO: force static modules on
endif()



# recurse
add_subdirectory(modules)
add_subdirectory(lib)
add_subdirectory(daemon)
