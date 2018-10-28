cmake_minimum_required(VERSION 2.8.8)
project(signaltk)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/extra/cmake")
set(CMAKE_CXX_STANDARD 11)

# INCLUDES
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# DEPENDENCIES
find_package(Threads)

find_package(Cairo)
include_directories(${CAIRO_INCLUDE_DIRS})

add_definitions(-DFNORDMETRIC_VERSION="unstable")

add_subdirectory(src/signaltk/core)
add_subdirectory(src/signaltk/plot)
add_subdirectory(src/signaltk/util)

add_executable(signaltk src/signaltk_cli.cc src/signaltk.cc)
target_link_libraries(signaltk signaltk-plot signaltk-graphics signaltk-util ${CAIRO_LIBRARIES})

add_subdirectory(test/plot)

