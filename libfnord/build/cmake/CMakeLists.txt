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
    stage/fnord/base/buffer.cc
    stage/fnord/base/datetime.cc
    stage/fnord/base/exception.cc
    stage/fnord/base/exceptionhandler.cc
    stage/fnord/base/inspect.cc
    stage/fnord/base/ieee754.cc
    stage/fnord/base/random.cc
    stage/fnord/base/stringutil.cc
    stage/fnord/base/uri.cc
    stage/fnord/base/wallclock.cc
    stage/fnord/hash/fnv.cc)

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
    stage/fnord/io/file.cc
    stage/fnord/io/fileutil.cc
    stage/fnord/io/filerepository.cc
    stage/fnord/io/inputstream.cc
    stage/fnord/io/outputstream.cc
    stage/fnord/io/mmappedfile.cc
    stage/fnord/io/pagemanager.cc)

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

# service: metric
add_library(fnord-metric-service OBJECT
    stage/fnord/service/metric/backends/disk/compactiontask.cc
    stage/fnord/service/metric/backends/disk/labelindex.cc
    stage/fnord/service/metric/backends/disk/labelindexreader.cc
    stage/fnord/service/metric/backends/disk/labelindexwriter.cc
    stage/fnord/service/metric/backends/disk/metric.cc
    stage/fnord/service/metric/backends/disk/metriccursor.cc
    stage/fnord/service/metric/backends/disk/metricrepository.cc
    stage/fnord/service/metric/backends/disk/metricsnapshot.cc
    stage/fnord/service/metric/backends/disk/samplereader.cc
    stage/fnord/service/metric/backends/disk/samplewriter.cc
    stage/fnord/service/metric/backends/disk/tableheaderreader.cc
    stage/fnord/service/metric/backends/disk/tableheaderwriter.cc
    stage/fnord/service/metric/backends/disk/tableref.cc
    stage/fnord/service/metric/backends/disk/tokenindex.cc
    stage/fnord/service/metric/backends/disk/tokenindexreader.cc
    stage/fnord/service/metric/backends/disk/tokenindexwriter.cc
    stage/fnord/service/metric/backends/inmemory/metric.cc
    stage/fnord/service/metric/backends/inmemory/metricrepository.cc
    stage/fnord/service/metric/metric.cc
    stage/fnord/service/metric/metricrepository.cc
    stage/fnord/service/metric/metricservice.cc
    stage/fnord/service/metric/metricserviceadapter.cc
    stage/fnord/service/metric/sample.cc)

add_executable(fnord-metric-service-example
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-metric-service>
    $<TARGET_OBJECTS:fnord-http>
    $<TARGET_OBJECTS:fnord-io>
    $<TARGET_OBJECTS:fnord-json>
    $<TARGET_OBJECTS:fnord-system>
    $<TARGET_OBJECTS:fnord-sstable>
    $<TARGET_OBJECTS:fnord-thread>
    $<TARGET_OBJECTS:fnord-util>
    stage/fnord/service/metric/metricservice_example.cc)

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

# service: keyvalue
add_library(fnord-keyvalue-service OBJECT
    stage/fnord/service/keyvalue/keyvalueservice.cc
    stage/fnord/service/keyvalue/keyvalueserviceadapter.cc)

add_executable(fnord-keyvalue-service-example
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-keyvalue-service>
    $<TARGET_OBJECTS:fnord-http>
    $<TARGET_OBJECTS:fnord-io>
    $<TARGET_OBJECTS:fnord-json>
    $<TARGET_OBJECTS:fnord-system>
    $<TARGET_OBJECTS:fnord-thread>
    stage/fnord/service/keyvalue/keyvalueservice_example.cc)

# service: ping
add_library(fnord-ping-service OBJECT
    stage/fnord/service/ping/pingservice.cc
    stage/fnord/service/ping/pingserviceadapter.cc)

add_executable(fnord-ping-service-example
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-ping-service>
    $<TARGET_OBJECTS:fnord-http>
    $<TARGET_OBJECTS:fnord-io>
    $<TARGET_OBJECTS:fnord-json>
    $<TARGET_OBJECTS:fnord-system>
    $<TARGET_OBJECTS:fnord-thread>
    stage/fnord/service/ping/pingservice_example.cc)

# sstable
add_library(fnord-sstable OBJECT
    stage/fnord/sstable/cursor.cc
    stage/fnord/sstable/fileheaderreader.cc
    stage/fnord/sstable/fileheaderwriter.cc
    stage/fnord/sstable/index.cc
    stage/fnord/sstable/indexprovider.cc
    stage/fnord/sstable/rowoffsetindex.cc
    stage/fnord/sstable/sstablereader.cc
    stage/fnord/sstable/sstablerepair.cc
    stage/fnord/sstable/sstablewriter.cc)

#sstable/sstable_test.cc

# system
add_library(fnord-system OBJECT
    stage/fnord/system/signalhandler.cc)

# thread
add_library(fnord-thread OBJECT
    stage/fnord/thread/threadpool.cc)

# util
add_library(fnord-util OBJECT
    stage/fnord/util/binarymessagereader.cc
    stage/fnord/util/binarymessagewriter.cc)
