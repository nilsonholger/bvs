project(BVSEXAMPLEMODULE)

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/BVSExampleModule.conf ${CMAKE_BINARY_DIR}/bin/BVSExampleModule.conf)

add_library(BVSExampleModule MODULE BVSExampleModule.cc)
target_link_libraries(BVSExampleModule bvs)
