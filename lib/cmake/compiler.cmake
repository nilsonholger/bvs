# STANDARDS
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -std=c99")

execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
if (GCC_VERSION VERSION_GREATER 4.6)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

if(${CMAKE_CXX_COMPILER} MATCHES "clang")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wc++11-extensions")
endif()



# WARNINGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Weffc++ -pedantic -pedantic-errors")



# PROFILING
if(USE_GPROF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif()
