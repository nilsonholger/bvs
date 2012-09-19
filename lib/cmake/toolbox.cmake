if(NOT TOOLBOX_INCLUDED)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}")
	include(compiler)
	include(macros)
	include(targets)
	set(TOOLBOX_INCLUDED ON)
endif()

