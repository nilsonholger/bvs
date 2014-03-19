project(BVSEXAMPLEMODULE)

execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/BVSExampleModule.conf ${CMAKE_BINARY_DIR}/bin/BVSExampleModule.conf)

set(ENV{BVS_STATIC_MODULES} "$ENV{BVS_STATIC_MODULES};BVSExampleModule")
add_library(BVSExampleModule ${BVS_MODULE_TYPE} BVSExampleModule.cc)
target_link_libraries(BVSExampleModule ${BVS_LINK_LIBRARIES})

if(NOT BVS_ANDROID_APP)
	#target_link_libraries(BVSExampleModule ...)
else()
	target_link_libraries(BVSExampleModule opencv_java log)
endif()
