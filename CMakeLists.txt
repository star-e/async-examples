# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.20)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project("async")

# map cmake config
set(CMAKE_MAP_IMPORTED_CONFIG_MINSIZEREL Release)
set(CMAKE_MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release)

if(WIN32)
  add_definitions(-D_MBCS)
endif()

# config c++ compiler
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Include sub-projects.
add_subdirectory("External/libunifex")
