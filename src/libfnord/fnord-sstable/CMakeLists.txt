# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# FnordMetric is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3.0. You should have received a
# copy of the GNU General Public License along with this program. If not, see
# <http://www.gnu.org/licenses/>.
cmake_minimum_required(VERSION 2.8.8)
project(libfnord-sstable)
include(FindPkgConfig)

option(ENABLE_TESTS "Build unit tests [default: ON]" ON)

include_directories(${PROJECT_BINARY_DIR})
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
include_directories(../)
include_directories(../3rdparty/)

find_package(Threads)

if(APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

# sstable
add_library(fnord-sstable OBJECT
    cursor.cc
    fileheaderreader.cc
    fileheaderwriter.cc
    index.cc
    indexprovider.cc
    rowoffsetindex.cc
    sstablereader.cc
    sstablerepair.cc
    sstablewriter.cc
    SSTableScan.cc
    SSTableColumnSchema.cc
    SSTableColumnReader.cc
    SSTableColumnWriter.cc)

add_executable(fn-sstablescan
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-sstable>
    fn-sstablescan.cc)

target_link_libraries(fn-sstablescan ${CMAKE_THREAD_LIBS_INIT})
