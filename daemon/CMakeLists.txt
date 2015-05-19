cmake_minimum_required(VERSION 2.8.6)

project(BVSD)

add_subdir_exec(src bvsd bvsd.cc)
if(BVS_STATIC)
	target_link_full_static_libraries(bvsd bvs $ENV{BVS_STATIC_MODULES})
else()
	target_link_libraries(bvsd bvs)
endif(BVS_STATIC)
