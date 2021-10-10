cmake_minimum_required(VERSION 2.8)

project(pd-mipt-hw-1)

set(CMAKE_CXX_COMPILER mpic++)
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/)
set(PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/)

option(VERBOSE "Verbose" OFF)
if(VERBOSE)
  set(CMAKE_CXX_FLAGS "-DVERBOSE ${CMAKE_CXX_FLAGS}")
endif()

add_subdirectory(src)
