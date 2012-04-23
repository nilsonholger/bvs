project(INPUT)

add_library(INPUT MODULE INPUT.cc)
target_link_libraries(INPUT bvs)
