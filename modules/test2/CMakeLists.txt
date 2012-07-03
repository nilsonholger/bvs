project(TEST2)

add_library(test2 MODULE test2.cc)
target_link_libraries(test2 bvs)
