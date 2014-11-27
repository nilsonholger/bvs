project(BVSEXAMPLEMODULE)

create_symlink(${CMAKE_CURRENT_SOURCE_DIR}/BVSExampleModule.conf ${CMAKE_BINARY_DIR}/bin/BVSExampleModule.conf)
add_bvs_module(BVSExampleModule BVSExampleModule.cc)

if(NOT BVS_ANDROID_APP)
	#target_link_libraries(BVSExampleModule ...)
else()
	#target_link_libraries(BVSExampleModule opencv_java log)
endif()
