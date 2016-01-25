# DEBUG INFORMATION
if(${CMAKE_BUILD_TYPE} STREQUAL DEBUG)
	if (APPLE)
		set(CMAKE_CXX_FLAGS_DEBUG "-g3 -ggdb3 -gdwarf-3")
	elseif(UNIX)
		set(CMAKE_CXX_FLAGS_DEBUG "-g3 -ggdb3 -gdwarf-4")
	endif()
endif()

# COMPILER DEPENDANT C++0x/C++11/C++14 FLAGS
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
	execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
	if (GCC_VERSION VERSION_GREATER 4.8)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	elseif (GCC_VERSION VERSION_GREATER 4.7)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
	elseif (GCC_VERSION VERSION_GREATER 4.6)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	endif()
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
	#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wc++11-extensions")
endif()

# WARNINGS
set(COMPILER_WARNINGS ON CACHE BOOL "Enable all/pedantic/effc++ compiler errors/warnings.")
mark_as_advanced(COMPILER_WARNINGS)
if(COMPILER_WARNINGS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -Weffc++ -Wpedantic -pedantic-errors")
endif()

set(COMPILER_ADVANCED OFF CACHE BOOL "Enable some advanced compiler options, currently: -pipe -fPIC -fstack-protector -Wl,-z,relro -Wl,-z,now")
mark_as_advanced(COMPILER_ADVANCED)
if(COMPILER_ADVANCED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -fPIC -fstack-protector -Wstack-protector -Wl,-z,relro -Wl,-z,now")
endif()

# WARNINGS MACRO
set(COMPILER_WARNINGS_MACRO ON CACHE BOOL "Macro to allow selective disabling of compiler warnings: disable_compiler_warnings([RECURSE] GLOB_LIST).")
mark_as_advanced(COMPILER_WARNINGS_MACRO)

# CPU/ARCH TUNING
set(COMPILER_TUNING OFF CACHE BOOL "Enable automatic architecture and cpu tuning.")
mark_as_advanced(COMPILER_TUNING)
if(COMPILER_TUNING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

# PROFILING
set(COMPILER_PROFILING OFF CACHE BOOL "Create profiled build for gprof.")
mark_as_advanced(COMPILER_PROFILING)
if(COMPILER_PROFILING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif()
