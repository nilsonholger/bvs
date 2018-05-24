cmake_minimum_required(VERSION 2.8.6)

project(BVS)
set(CMAKE_BUILD_TYPE "DEBUG")
include(cmake/toolbox/toolbox.cmake)
include(cmake/bvs-options.cmake)
include(cmake/bvs-macros.cmake)

# dynamic library stuff
if(${APPLE})
	cmake_minimum_required(VERSION 3.0.2)
	set(CMAKE_MACOSX_RPATH ON)
else()
	set(CMAKE_SKIP_RPATH ON)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-rpath,.")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# output directory
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "" FORCE)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE PATH "" FORCE)

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

# includes
include_directories(lib/include)
if(BVS_ANDROID_APP)
	include_directories(android)
endif()

# subdirectories
add_subdirectory(modules)
if(BVS_ANDROID_APP)
	add_subdirectory(android)
else()
	add_subdirectory(lib)
	add_subdirectory(daemon)
endif()
