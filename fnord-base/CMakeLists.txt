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
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
include_directories(../)
include_directories(../3rdparty/)

find_package(Threads)

if(APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

# base
add_library(fnord-base OBJECT
    application.cc
    autoref.cc
    assets.cc
    buffer.cc
    bufferutil.cc
    cli/flagparser.cc
    datetime.cc
    duration.cc
    exception.cc
    exceptionhandler.cc
    fnv.cc
    ieee754.cc
    inspect.cc
    io/file.cc
    io/fileutil.cc
    io/filerepository.cc
    io/inputstream.cc
    io/outputstream.cc
    io/mmappedfile.cc
    io/pagemanager.cc
    logging.cc
    logging/logoutputstream.cc
    random.cc
    status.cc
    stringutil.cc
    thread/eventloop.cc
    thread/signalhandler.cc
    thread/threadpool.cc
    thread/wakeup.cc
    uri.cc
    wallclock.cc)

#add_executable(tests/test-inputstream
#    $<TARGET_OBJECTS:fnord-base>
#    $<TARGET_OBJECTS:fnord-io>
#    io/inputstream_test.cc)

# add_executable(tests/test-uri
#     $<TARGET_OBJECTS:fnord-base>
#     base/uri_test.cc)
# 
# add_executable(tests/test-stringutil
#     $<TARGET_OBJECTS:fnord-base>
#     base/stringutil_test.cc)

# chart
#add_library(fnord-chart OBJECT
#    chart/axisdefinition.cc
#    chart/areachart.cc
#    chart/barchart.cc
#    chart/linechart.cc
#    chart/pointchart.cc
#    chart/canvas.cc
#    chart/domain.cc
#    chart/domainprovider.cc
#    chart/drawable.cc
#    chart/griddefinition.cc
#    chart/legenddefinition.cc
#    chart/series.cc
#    chart/timedomain.cc)

#add_executable(tests/test-chart
#    $<TARGET_OBJECTS:fnord-base>
#    $<TARGET_OBJECTS:fnord-chart>
#    chart/chart_test.cc)
#
target_link_libraries(tests/test-chart ${CMAKE_THREAD_LIBS_INIT})

# http
#add_library(fnord-http OBJECT
#    net/http/cookies.cc
#    net/http/httpclient.cc
#    net/http/httpclientconnection.cc
#    net/http/httpconnectionpool.cc
#    net/http/httpgenerator.cc
#    net/http/httpmessage.cc
#    net/http/httpparser.cc
#    net/http/httprequest.cc
#    net/http/httpresponse.cc
#    net/http/httpresponsefuture.cc
#    net/http/httprouter.cc
#    net/http/httpserver.cc
#    net/http/httpserverconnection.cc
#    net/http/httpservice.cc)
#
#add_executable(tests/test-http
#    $<TARGET_OBJECTS:fnord-base>
#    $<TARGET_OBJECTS:fnord-comm>
#    $<TARGET_OBJECTS:fnord-net>
#    $<TARGET_OBJECTS:fnord-http>
#    $<TARGET_OBJECTS:fnord-stats>
#    net/http/http_test.cc)
#
#target_link_libraries(tests/test-http ${CMAKE_THREAD_LIBS_INIT})

# json
add_library(fnord-json OBJECT
    json/flatjsonreader.cc
    json/json.cc
    json/jsondocument.cc
    json/jsoninputstream.cc
    json/jsonoutputstream.cc
    json/jsonpointer.cc
    json/jsonrpc.cc
    json/jsonrpchttpadapter.cc
    json/jsonrpcrequest.cc
    json/jsonrpcresponse.cc
    json/jsonutil.cc)

add_executable(tests/test-json
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-comm>
    $<TARGET_OBJECTS:fnord-http>
    $<TARGET_OBJECTS:fnord-net>
    $<TARGET_OBJECTS:fnord-json>
    $<TARGET_OBJECTS:fnord-stats>
    json/json_test.cc)

target_link_libraries(tests/test-json ${CMAKE_THREAD_LIBS_INIT})

# service: metric
add_library(fnord-metric-service OBJECT
    net/statsd/statsd.cc
    service/metric/backends/disk/compactiontask.cc
    service/metric/backends/disk/labelindex.cc
    service/metric/backends/disk/labelindexreader.cc
    service/metric/backends/disk/labelindexwriter.cc
    service/metric/backends/disk/metric.cc
    service/metric/backends/disk/metriccursor.cc
    service/metric/backends/disk/metricrepository.cc
    service/metric/backends/disk/metricsnapshot.cc
    service/metric/backends/disk/samplereader.cc
    service/metric/backends/disk/samplewriter.cc
    service/metric/backends/disk/tableheaderreader.cc
    service/metric/backends/disk/tableheaderwriter.cc
    service/metric/backends/disk/tableref.cc
    service/metric/backends/disk/tokenindex.cc
    service/metric/backends/disk/tokenindexreader.cc
    service/metric/backends/disk/tokenindexwriter.cc
    service/metric/backends/inmemory/metric.cc
    service/metric/backends/inmemory/metricrepository.cc
    service/metric/httpapiservlet.cc
    service/metric/metric.cc
    service/metric/metricrepository.cc
    service/metric/metricservice.cc
    service/metric/timeseriesquery.cc
    service/metric/sample.cc)

add_executable(fnord-metric-service-example
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-chart>
    $<TARGET_OBJECTS:fnord-metric-service>
    $<TARGET_OBJECTS:fnord-http>
    $<TARGET_OBJECTS:fnord-net>
    $<TARGET_OBJECTS:fnord-comm>
    $<TARGET_OBJECTS:fnord-json>
    $<TARGET_OBJECTS:fnord-sstable>
    $<TARGET_OBJECTS:fnord-util>
    service/metric/metricservice_example.cc)

target_link_libraries(fnord-metric-service-example ${CMAKE_THREAD_LIBS_INIT})

# net
add_library(fnord-net OBJECT
    net/dnscache.cc
    net/tcpserver.cc
    net/udpserver.cc
    net/udpsocket.cc
    net/inetaddr.cc
    net/tcpconnection.cc)

# redis
add_library(fnord-redis OBJECT
    ../3rdparty/hiredis/async.c
    ../3rdparty/hiredis/dict.c
    ../3rdparty/hiredis/hiredis.c
    ../3rdparty/hiredis/net.c
    ../3rdparty/hiredis/sds.c
    net/redis/redisconnection.cc
    net/redis/redisqueue.cc)

#add_executable(tests/test-redis
#    $<TARGET_OBJECTS:fnord-base>
#    $<TARGET_OBJECTS:fnord-comm>
#    $<TARGET_OBJECTS:fnord-net>
#    $<TARGET_OBJECTS:fnord-redis>
#    net/redis/redis_test.cc)


# sstable
add_library(fnord-sstable OBJECT
    sstable/cursor.cc
    sstable/fileheaderreader.cc
    sstable/fileheaderwriter.cc
    sstable/index.cc
    sstable/indexprovider.cc
    sstable/rowoffsetindex.cc
    sstable/sstablereader.cc
    sstable/sstablerepair.cc
    sstable/sstablewriter.cc)

add_library(fnord-stats OBJECT
    stats/statshttpservlet.cc
    stats/statsdagent.cc
    stats/statsrepository.cc
    stats/statssink.cc)

# util
add_library(fnord-util OBJECT
    util/binarymessagereader.cc
    util/binarymessagewriter.cc)
