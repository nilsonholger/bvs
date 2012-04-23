project(OUTPUT)

add_library(OUTPUT MODULE OUTPUT.cc)
target_link_libraries(OUTPUT bvs)
