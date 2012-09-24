project(BVSEXAMPLEMODULE)

add_library(BVSExampleModule MODULE BVSExampleModule.cc)
target_link_libraries(BVSExampleModule bvs)
