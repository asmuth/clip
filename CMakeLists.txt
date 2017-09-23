cmake_minimum_required(VERSION 3.1)
project(libpaint)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)

add_library(zdb STATIC
    core/database.h
    core/database.cc
    core/metadata.h
    core/metadata.cc
    core/zdb.h
    core/zdb.cc)

add_executable(zdbtool
    core/util/exception.h
    core/util/exception.cc
    core/util/flagparser.h
    core/util/flagparser.cc
    core/util/stringutil.h
    core/util/stringutil.cc
    core/util/time.h
    core/util/time.cc
    tools/zdbtool.cc
    tools/zdbtool_init.cc)

add_executable(zdbtest
    core/util/exception.h
    core/util/exception.cc
    core/util/flagparser.h
    core/util/flagparser.cc
    core/util/stringutil.h
    core/util/stringutil.cc
    core/util/time.h
    core/util/time.cc
    test/smoketest.cc)

target_link_libraries(zdbtest zdb pthread)

