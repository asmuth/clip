cmake_minimum_required(VERSION 2.8.8)
project(libcplot)

# INCLUDES
set(MODULE_INCLUDE
    ${PROJECT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${MODULE_INCLUDE})
set(CPLOT_INCLUDE ${MODULE_INCLUDE} PARENT_SCOPE)

# DEPENDENCIES
find_package(Threads)

add_subdirectory(src/cplot)
