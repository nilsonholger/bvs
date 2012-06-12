project(TEST)

add_library(test MODULE test.cc)
target_link_libraries(test bvs)
