cmake_minimum_required(VERSION 2.8.6)

project(LIBBVS)
include(cmake/toolbox.cmake)

include_directories(include)
include_directories(src)
add_subdir_lib(src bvs SHARED bvs.cc bvsinfo.cc config.cc connector.cc control.cc loader.cc logger.cc logsystem.cc module.cc)
target_link_libraries(bvs dl pthread)

# add documentation target (using doxygen)
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(bvs-doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating documentation for libbvs!" VERBATIM)
endif()
