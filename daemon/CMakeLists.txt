project(BVSD)

add_subdir_exec(src bvsd BVSDaemon.cc)
target_link_libraries(bvsd bvs)
