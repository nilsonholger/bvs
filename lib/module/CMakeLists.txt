project(BVSEXAMPLEMODULE)

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/BVSExampleModuleConfig.txt ${CMAKE_BINARY_DIR}/bin/BVSExampleModuleConfig.txt)

add_library(BVSExampleModule MODULE BVSExampleModule.cc)
target_link_libraries(BVSExampleModule bvs)
