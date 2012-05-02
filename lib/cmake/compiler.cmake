cmake_minimum_required(VERSION 2.8.6)



#TODO for gcc-4.6 set -std=c++0x
# STANDARDS
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -std=c99")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if(${CMAKE_CXX_COMPILER} MATCHES "clang")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wc++11-extensions")
endif(${CMAKE_CXX_COMPILER} MATCHES "clang")



# WARNINGS
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Weffc++ -pedantic -pedantic-errors")



# PROFILING
if(USE_GPROF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif(USE_GPROF)
