# add dependeny between a variable and (a) condition(s)
#
# CALL: add_dependeny(OPT COND ...)
#   OPT: otpion/variable to disable
#   COND: conditional option to use
#   ...: condition(s), must be valid cmake condition, e.g. NOT (FOO OR BAR)
#
macro(add_dependency OPT COND)
	if(NOT ${COND} STREQUAL "ON_IF" AND NOT ${COND} STREQUAL "OFF_IF")
		message(FATAL_ERROR "wrong argument, must be: ON_IF or OFF_IF!")
	endif()
	set(COND_LIST ${ARGV})
	list(REMOVE_AT COND_LIST 0 1)
	if((NOT DEFINED __${OPT}) AND (NOT ${OPT} STREQUAL "${__${OPT}}"))
		get_property(HELPSTRING CACHE ${OPT} PROPERTY HELPSTRING)
		set(__${OPT} ${${OPT}} CACHE INTERNAL "${HELPSTRING}")
	endif()
	get_property(HELPSTRING CACHE __${OPT} PROPERTY HELPSTRING)
	if(${COND_LIST})
		unset(${OPT} CACHE)
		if(${COND} STREQUAL "ON_IF")
			set(${OPT} ON CACHE INTERNAL "${HELPSTRING}" FORCE)
		endif()
	else()
		set(${OPT} ${__${OPT}} CACHE BOOL "${HELPSTRING}" FORCE)
		unset(__${OPT} CACHE)
	endif()
endmacro(add_dependency)

# take a directory and a list of sources and prepend the directory to each source
#
# CALL: add_subdir(DIR SRC_LIST_NAME FILE1 [...])
#	DIR: the directory to use
#	SRC_LIST_NAME: the name of the variable to use for the list
#	FILE1 [...]: list of sources
#
macro(add_subdir DIR SRC_LIST_NAME)
	set(SRC_LIST ${ARGV})
	list(REMOVE_AT SRC_LIST 0 1)
	set(${SRC_LIST_NAME} "")
	foreach(I ${SRC_LIST})
		set(${SRC_LIST_NAME} ${${SRC_LIST_NAME}} ${DIR}/${I})
	endforeach(I)
endmacro(add_subdir)

# add_executable using a subdirectory
#
# CALL: add_subdir_exec(DIR EXEC_NAME SRC_LIST)
#	DIR: the directory to use
#	EXEC_NAME: the name of the executable
#	SRC_LIST: list of sources to use
#
macro(add_subdir_exec DIR EXEC_NAME)
	set(SRC_LIST ${ARGV})
	list(REMOVE_AT SRC_LIST 0 1)
	add_subdir(${DIR} SRC_FILES ${SRC_LIST})
	add_executable(${EXEC_NAME} ${SRC_FILES})
endmacro(add_subdir_exec)

# add_library using a subdirectory
#
# CALL: add_subdir_lib(DIR LIB_NAME LIB_TYPE SRC_LIST)
#	DIR: the directory to use
#	LIB_NAME: the name of the library
#	LIB_TYPE: the type [STATIC|SHARED|MODULE]
#	SRC_LIST: list of sources to use
#
macro(add_subdir_lib DIR LIB_NAME LIB_TYPE)
	set(SRC_LIST ${ARGV})
	list(REMOVE_AT SRC_LIST 0 1 2)
	add_subdir(${DIR} SRC_FILES ${SRC_LIST})
	add_library(${LIB_NAME} ${LIB_TYPE} ${SRC_FILES})
endmacro(add_subdir_lib)

# disable compiler warnings (only works when COMPILER_WARNINGS and COMPILER_WARNINGS_MACRO is enabled)
#
# CALL: disable_compiler_warnings([RECURSE] GLOB_LIST)
#	RECURSE: OPTIONAL use recursive glob
#	GLOB_LIST: list of globbing expressions (e.g. *cpp subdir/*cpp)
#
macro(disable_compiler_warnings RECURSE)
	if(COMPILER_WARNINGS AND COMPILER_WARNINGS_MACRO)
		set(GLOB_LIST ${ARGV})
		if(${RECURSE} STREQUAL "RECURSE")
			list (REMOVE_AT ${GLOB_LIST} 0)
			file(GLOB_RECURSE GLOB_FILES ${GLOB_LIST})
		else()
			file(GLOB GLOB_FILES ${GLOB_LIST})
		endif()
		set_source_files_properties(${GLOB_FILES} PROPERTIES COMPILE_FLAGS -w)
	endif()
endmacro(disable_compiler_warnings)

# display all set variables
#
# CALL: display_all_variables()
macro(display_all_variables)
	get_cmake_property(_variableNames VARIABLES)
	foreach (_variableName ${_variableNames})
		message(STATUS "${_variableName} = ${${_variableName}}")
	endforeach()
endmacro(display_all_variables)
