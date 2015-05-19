cmake_minimum_required(VERSION 2.8.6)

project(LIBBVS)

include_directories(include src)
add_subdir_lib(src bvs ${BVS_LIBRARY_TYPE} barrier.cc bvs.cc config.cc connector.cc control.cc info.cc loader.cc logger.cc logsystem.cc module.cc utils.cc)
target_link_libraries(bvs dl pthread)

if(BVS_STATIC_MODULES)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_link_libraries(bvs -Wl,--whole-archive $ENV{BVS_STATIC_MODULES} -Wl,--no-whole-archive)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		target_link_libraries(bvs -Wl,-all_load $ENV{BVS_STATIC_MODULES})
	endif()
endif()

if(NOT EXISTS ${CMAKE_BINARY_DIR}/bin/bvs.conf)
	execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/bvs.conf ${CMAKE_BINARY_DIR}/bin/bvs.conf)
endif()

# add documentation target (using doxygen)
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(bvs-doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating documentation for libbvs!" VERBATIM)
else()
	add_custom_target(bvs-doc echo "DOXYGEN NOT FOUND: it is required to build the documentation! Please consider installing it.")
endif()
