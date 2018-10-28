cmake_minimum_required(VERSION 2.8.8)
project(signaltk)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/extra/cmake")
set(CMAKE_CXX_STANDARD 11)

# INCLUDES
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# DEPENDENCIES
find_package(Threads)

find_package(Cairo)
include_directories(${CAIRO_INCLUDE_DIRS})

add_subdirectory(core/plot)
add_subdirectory(core/util)
add_subdirectory(core/graphics)

add_subdirectory(test/plot)

