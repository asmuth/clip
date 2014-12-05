# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# FnordMetric is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3.0. You should have received a
# copy of the GNU General Public License along with this program. If not, see
# <http://www.gnu.org/licenses/>.
cmake_minimum_required(VERSION 2.8.8)
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

# base
add_library(fnord-base OBJECT
    stage/fnord/base/assets.cc
    stage/fnord/base/datetime.cc
    stage/fnord/base/exception.cc
    stage/fnord/base/exceptionhandler.cc
    stage/fnord/base/inspect.cc
    stage/fnord/base/random.cc
    stage/fnord/base/stringutil.cc
    stage/fnord/base/uri.cc
    stage/fnord/base/wallclock.cc)

add_executable(tests/test-uri
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-io>
    stage/fnord/base/uri_test.cc)

add_executable(tests/test-stringutil
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-io>
    stage/fnord/base/stringutil_test.cc)

# chart
add_library(fnord-chart OBJECT
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
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-io>
    $<TARGET_OBJECTS:fnord-chart>
    stage/fnord/chart/chart_test.cc)

# http
add_library(fnord-http OBJECT
    stage/fnord/net/http/httpinputstream.cc
    stage/fnord/net/http/httpmessage.cc
    stage/fnord/net/http/httpoutputstream.cc
    stage/fnord/net/http/httprequest.cc
    stage/fnord/net/http/httpresponse.cc
    stage/fnord/net/http/httpserver.cc)

add_executable(tests/test-http
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-io>
    $<TARGET_OBJECTS:fnord-http>
    stage/fnord/net/http/http_test.cc)

# io
add_library(fnord-io OBJECT
    stage/fnord/io/fileutil.cc
    stage/fnord/io/inputstream.cc
    stage/fnord/io/outputstream.cc)

#add_executable(tests/test-inputstream
#    $<TARGET_OBJECTS:fnord-base>
#    $<TARGET_OBJECTS:fnord-io>
#    stage/fnord/io/inputstream_test.cc)

# json
add_library(fnord-json OBJECT
    stage/fnord/json/flatjsonreader.cc
    stage/fnord/json/jsondocument.cc
    stage/fnord/json/jsoninputstream.cc
    stage/fnord/json/jsonoutputstream.cc
    stage/fnord/json/jsonpointer.cc
    stage/fnord/json/jsonrpc.cc
    stage/fnord/json/jsonrpchttpadapter.cc
    stage/fnord/json/jsonrpcrequest.cc
    stage/fnord/json/jsonrpcresponse.cc)

add_executable(tests/test-json
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-http>
    $<TARGET_OBJECTS:fnord-io>
    $<TARGET_OBJECTS:fnord-json>
    stage/fnord/json/json_test.cc)

# service: groups
add_library(fnord-groups-service OBJECT
    stage/fnord/service/groups/groupsservice.cc
    stage/fnord/service/groups/groupsserviceadapter.cc)

add_executable(fnord-groups-service-example
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-groups-service>
    $<TARGET_OBJECTS:fnord-http>
    $<TARGET_OBJECTS:fnord-io>
    $<TARGET_OBJECTS:fnord-json>
    $<TARGET_OBJECTS:fnord-system>
    $<TARGET_OBJECTS:fnord-thread>
    stage/fnord/service/groups/groupsservice_example.cc)

# system
add_library(fnord-system OBJECT
    stage/fnord/system/signalhandler.cc)

# thread
add_library(fnord-thread OBJECT
    stage/fnord/thread/threadpool.cc)
