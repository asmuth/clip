# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# FnordMetric is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3.0. You should have received a
# copy of the GNU General Public License along with this program. If not, see
# <http://www.gnu.org/licenses/>.
cmake_minimum_required(VERSION 2.8.8)
project(libfnord-metricb)
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

add_library(fnord-metricdb OBJECT
    backends/disk/compactiontask.cc
    backends/disk/labelindex.cc
    backends/disk/labelindexreader.cc
    backends/disk/labelindexwriter.cc
    backends/disk/metric.cc
    backends/disk/metriccursor.cc
    backends/disk/metricrepository.cc
    backends/disk/metricsnapshot.cc
    backends/disk/samplereader.cc
    backends/disk/samplewriter.cc
    backends/disk/tableheaderreader.cc
    backends/disk/tableheaderwriter.cc
    backends/disk/tableref.cc
    backends/disk/tokenindex.cc
    backends/disk/tokenindexreader.cc
    backends/disk/tokenindexwriter.cc
    backends/inmemory/metric.cc
    backends/inmemory/metricrepository.cc
    httpapiservlet.cc
    metric.cc
    metricrepository.cc
    metricservice.cc
    timeseriesquery.cc
    sample.cc)

add_executable(fnord-metricdb-example
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-chart>
    $<TARGET_OBJECTS:fnord-metricdb>
    $<TARGET_OBJECTS:fnord-http>
    $<TARGET_OBJECTS:fnord-json>
    $<TARGET_OBJECTS:fnord-sstable>
    metricservice_example.cc)

target_link_libraries(fnord-metricdb-example ${CMAKE_THREAD_LIBS_INIT})

