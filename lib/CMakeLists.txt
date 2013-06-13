cmake_minimum_required(VERSION 2.8.6)

project(LIBBVS)
include(cmake/toolbox.cmake)

include_directories(include src)
add_subdir_lib(src bvs SHARED barrier.cc bvs.cc config.cc connector.cc control.cc info.cc loader.cc logger.cc logsystem.cc module.cc utils.cc)
target_link_libraries(bvs dl pthread)

set(BVS_LIBRARY_TYPE STATIC PARENT_SCOPE)
set(BVS_LINK_LIBRARY bvs PARENT_SCOPE)

if(BVS_STATIC_MODULES)
target_link_libraries(bvs -Wl,--whole-archive $ENV{BVS_STATIC_MODULES} -Wl,--no-whole-archive)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/bvs.conf ${CMAKE_BINARY_DIR}/bin/bvs.conf)

# add documentation target (using doxygen)
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(bvs-doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating documentation for libbvs!" VERBATIM)
else()
	add_custom_target(bvs-doc echo "DOXYGEN NOT FOUND: it is required to build the documentation! Please consider installing it.")
endif()

# BVS_MODULE_HOTSWAP
set(BVS_MODULE_HOTSWAP OFF CACHE BOOL "Enable BVS's HotSwap(TM;-) facilities. WARNING: DEVELOPERS ONLY!")
mark_as_advanced(BVS_MODULE_HOTSWAP)
if(BVS_MODULE_HOTSWAP)
	add_definitions(-DBVS_MODULE_HOTSWAP)
endif()

# BVS_GCC_VISIBILITY
set(BVS_GCC_VISIBILITY ON CACHE BOOL "Enable gcc's visibility feature (reduce dynamic library load times as well as decrease library size).")
mark_as_advanced(BVS_GCC_VISIBILITY)
if(BVS_GCC_VISIBILITY)
	add_definitions(-DBVS_GCC_VISIBILITY -fvisibility=hidden)
endif()

# BVS_OSX_ANOMALIES
set(BVS_OSX_ANOMALIES ${APPLE} CACHE BOOL "Enable OSX anomalies, e.g. 'so->dylib'.")
mark_as_advanced(BVS_OSX_ANOMALIES)
if(BVS_OSX_ANOMALIES)
	add_definitions(-DBVS_OSX_ANOMALIES)
endif()



# DEPENDENCIES
add_dependency(BVS_MODULE_HOTSWAP OFF_IF BVS_STATIC_MODULES)
