project(BVSEXAMPLEMODULE)

add_library(BVSExampleModule SHARED BVSExampleModule.cc)
target_link_libraries(BVSExampleModule bvs)
