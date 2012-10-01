# STANDARDS
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -std=c99")

execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
	if (GCC_VERSION VERSION_GREATER 4.6)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	endif()
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wc++11-extensions")
endif()



# WARNINGS
set(COMPILER_WARNINGS ON CACHE BOOL "Enable all/pedantic/effc++ compiler warnings.")
if(COMPILER_WARNINGS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Weffc++ -pedantic -pedantic-errors")
endif()

# CPU/ARCH TUNING
set(COMPILER_TUNING OFF CACHE BOOL "Enable automatic architecture and cpu tuning.")
if(COMPILER_TUNING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

# PROFILING
set(COMPILER_PROFILING OFF CACHE BOOL "Create profiled build for gprof.")
if(COMPILER_PROFILING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif()
