cmake_minimum_required(VERSION 2.8.6)

# add bvs module
#
# CALL: add_bvs_module(MODULE_NAME SRC_LIST)
macro(add_bvs_module MODULE_NAME)
	set(SRC_LIST ${ARGV})
	list(REMOVE_AT SRC_LIST 0)
	set(ENV{BVS_STATIC_MODULES} "$ENV{BVS_STATIC_MODULES};${MODULE_NAME}")
	add_library(${MODULE_NAME} ${BVS_MODULE_TYPE} ${SRC_LIST})
	target_link_libraries(${MODULE_NAME} ${BVS_LINK_LIBRARIES})
endmacro()
