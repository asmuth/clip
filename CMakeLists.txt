cmake_minimum_required(VERSION 3.1)
project(libpaint)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(zdb STATIC
    src/zdb/database.h
    src/zdb/database.cc
    src/zdb/metadata.h
    src/zdb/metadata.cc
    src/zdb/zdb.h
    src/zdb/zdb.cc)

add_executable(zdbtool
    src/zdb/util/exception.h
    src/zdb/util/exception.cc
    src/zdb/util/flagparser.h
    src/zdb/util/flagparser.cc
    src/zdb/util/stringutil.h
    src/zdb/util/stringutil.cc
    src/zdb/util/time.h
    src/zdb/util/time.cc
    src/zdb/zdbtool.cc
    src/zdb/zdbtool_init.cc)

add_executable(zdbtest
    src/zdb/util/exception.h
    src/zdb/util/exception.cc
    src/zdb/util/flagparser.h
    src/zdb/util/flagparser.cc
    src/zdb/util/stringutil.h
    src/zdb/util/stringutil.cc
    src/zdb/util/time.h
    src/zdb/util/time.cc
    test/smoketest.cc)

target_link_libraries(zdbtest zdb pthread)

