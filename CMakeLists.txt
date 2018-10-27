cmake_minimum_required(VERSION 2.8.8)
project(signaltk)

# INCLUDES
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# DEPENDENCIES
find_package(Threads)

add_subdirectory(core/plot)
add_subdirectory(core/util)

add_subdirectory(test/plot)
