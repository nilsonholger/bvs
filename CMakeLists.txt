cmake_minimum_required(VERSION 2.8.6)

project(BVSBASE)
set(CMAKE_BUILD_TYPE "DEBUG")
include(lib/cmake/toolbox.cmake)

# general settings
include_directories(lib/include)
set(CMAKE_SKIP_RPATH ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath,.")

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

# recurse
add_subdirectory(lib)
add_subdirectory(daemon)
add_subdirectory(modules)
