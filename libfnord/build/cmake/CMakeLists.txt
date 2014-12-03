# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# FnordMetric is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3.0. You should have received a
# copy of the GNU General Public License along with this program. If not, see
# <http://www.gnu.org/licenses/>.
cmake_minimum_required(VERSION 2.6)
include(FindPkgConfig)
include(CheckIncludeFile)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/") 
project(libfnord)

option(ENABLE_TESTS "Build unit tests [default: ON]" ON)

include_directories(${PROJECT_BINARY_DIR})
include_directories(stage/)

if(APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

# chart
set(FNORD_CHART_SOURCES
    stage/fnord/chart/axisdefinition.cc
    stage/fnord/chart/areachart.cc
    stage/fnord/chart/barchart.cc
    stage/fnord/chart/linechart.cc
    stage/fnord/chart/pointchart.cc
    stage/fnord/chart/canvas.cc
    stage/fnord/chart/domain.cc
    stage/fnord/chart/domainprovider.cc
    stage/fnord/chart/drawable.cc
    stage/fnord/chart/griddefinition.cc
    stage/fnord/chart/legenddefinition.cc
    stage/fnord/chart/series.cc
    stage/fnord/chart/timedomain.cc)

add_executable(tests/test-chart
    ${FNORD_CHART_SOURCES}
    stage/fnord/chart/chart_test.cc)

