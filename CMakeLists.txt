cmake_minimum_required(VERSION 2.8.6)

project(BVSBASE)
set(CMAKE_BUILD_TYPE "DEBUG")
include(libbvs/cmake/toolbox.cmake)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# add documentation target (using doxygen)
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating documentation for libbvs!" VERBATIM)
endif()

# BVS_THREAD_NAMES
set(BVS_THREAD_NAMES OFF CACHE BOOL "Enable thread naming (might not show in every version of (h)top, etc, try 'ps -La'). UNIX ONLY!")
if(BVS_THREAD_NAMES)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBVS_THREAD_NAMES")
endif()

include_directories(libbvs/include)
add_subdirectory(libbvs)
add_subdirectory(bvsd)
add_subdirectory(modules)
