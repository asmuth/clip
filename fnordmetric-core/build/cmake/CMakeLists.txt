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
project(fnordmetric)

option(ENABLE_TESTS "Build unit tests [default: off]" OFF)

set(FNORDMETRIC_SOURCES
    stage/src/fnordmetric/cli/cli.cc
    stage/src/fnordmetric/cli/flagparser.cc
    stage/src/fnordmetric/cli/cli.cc
    stage/src/fnordmetric/io/acceptor.cc
    stage/src/fnordmetric/io/eventloop.cc
    stage/src/fnordmetric/io/file.cc
    stage/src/fnordmetric/io/fileutil.cc
    stage/src/fnordmetric/io/filerepository.cc
    stage/src/fnordmetric/io/mmappedfile.cc
    stage/src/fnordmetric/io/pagemanager.cc
    stage/src/fnordmetric/net/udpserver.cc
    stage/src/fnordmetric/environment.cc
    stage/src/fnordmetric/http/httpinputstream.cc
    stage/src/fnordmetric/http/httpoutputstream.cc
    stage/src/fnordmetric/http/httpmessage.cc
    stage/src/fnordmetric/http/httprequest.cc
    stage/src/fnordmetric/http/httpresponse.cc
    stage/src/fnordmetric/http/httpserver.cc
    stage/src/fnordmetric/sstable/cursor.cc
    stage/src/fnordmetric/sstable/fileheaderreader.cc
    stage/src/fnordmetric/sstable/fileheaderwriter.cc
    stage/src/fnordmetric/sstable/index.cc
    stage/src/fnordmetric/sstable/indexprovider.cc
    stage/src/fnordmetric/sstable/rowoffsetindex.cc
    stage/src/fnordmetric/sstable/sstablereader.cc
    stage/src/fnordmetric/sstable/sstablerepair.cc
    stage/src/fnordmetric/sstable/sstablewriter.cc
    stage/src/fnordmetric/util/assets.cc
    stage/src/fnordmetric/util/binarymessagereader.cc
    stage/src/fnordmetric/util/binarymessagewriter.cc
    stage/src/fnordmetric/util/buffer.cc
    stage/src/fnordmetric/util/datetime.cc
    stage/src/fnordmetric/util/exceptionhandler.cc
    stage/src/fnordmetric/util/format.cc
    stage/src/fnordmetric/util/fnv.cc
    stage/src/fnordmetric/util/ieee754.cc
    stage/src/fnordmetric/util/inputstream.cc
    stage/src/fnordmetric/util/inspect.cc
    stage/src/fnordmetric/util/logger.cc
    stage/src/fnordmetric/util/logoutputstream.cc
    stage/src/fnordmetric/util/outputstream.cc
    stage/src/fnordmetric/util/jsonoutputstream.cc
    stage/src/fnordmetric/util/random.cc
    stage/src/fnordmetric/util/runtimeexception.cc
    stage/src/fnordmetric/util/signalhandler.cc
    stage/src/fnordmetric/util/stringutil.cc
    stage/src/fnordmetric/util/uri.cc
    stage/src/fnordmetric/util/wallclock.cc
    stage/src/fnordmetric/ui/axisdefinition.cc
    stage/src/fnordmetric/ui/areachart.cc
    stage/src/fnordmetric/ui/barchart.cc
    stage/src/fnordmetric/ui/linechart.cc
    stage/src/fnordmetric/ui/pointchart.cc
    stage/src/fnordmetric/ui/canvas.cc
    stage/src/fnordmetric/ui/domain.cc
    stage/src/fnordmetric/ui/domainprovider.cc
    stage/src/fnordmetric/ui/drawable.cc
    stage/src/fnordmetric/ui/griddefinition.cc
    stage/src/fnordmetric/ui/legenddefinition.cc
    stage/src/fnordmetric/ui/series.cc
    stage/src/fnordmetric/ui/timedomain.cc
    stage/src/fnordmetric/sql/backends/csv/csvbackend.cc
    stage/src/fnordmetric/sql/backends/csv/csvinputstream.cc
    stage/src/fnordmetric/sql/backends/csv/csvtableref.cc
    stage/src/fnordmetric/sql/backends/mysql/mysqlbackend.cc
    stage/src/fnordmetric/sql/backends/mysql/mysqlconnection.cc
    stage/src/fnordmetric/sql/backends/mysql/mysqltableref.cc
    stage/src/fnordmetric/sql/backends/crate/cratebackend.cc
    stage/src/fnordmetric/sql/backends/crate/crateconnection.cc
    stage/src/fnordmetric/sql/backends/crate/cratetableref.cc
    stage/src/fnordmetric/query/query.cc
    stage/src/fnordmetric/query/queryservice.cc
    stage/src/fnordmetric/sql/expressions/aggregate.cc
    stage/src/fnordmetric/sql/expressions/boolean.cc
    stage/src/fnordmetric/sql/expressions/datetime.cc
    stage/src/fnordmetric/sql/expressions/math.cc
    stage/src/fnordmetric/sql/parser/astnode.cc
    stage/src/fnordmetric/sql/parser/astutil.cc
    stage/src/fnordmetric/sql/parser/parser.cc
    stage/src/fnordmetric/sql/parser/token.cc
    stage/src/fnordmetric/sql/parser/tokenize.cc
    stage/src/fnordmetric/sql/runtime/compile.cc
    stage/src/fnordmetric/sql/runtime/defaultruntime.cc
    stage/src/fnordmetric/sql/runtime/execute.cc
    stage/src/fnordmetric/sql/runtime/groupovertimewindow.cc
    stage/src/fnordmetric/sql/runtime/orderby.cc
    stage/src/fnordmetric/sql/runtime/importstatement.cc
    stage/src/fnordmetric/sql/runtime/queryplan.cc
    stage/src/fnordmetric/sql/runtime/queryplanbuilder.cc
    stage/src/fnordmetric/sql/runtime/queryplannode.cc
    stage/src/fnordmetric/sql/runtime/runtime.cc
    stage/src/fnordmetric/sql/runtime/symboltable.cc
    stage/src/fnordmetric/sql/runtime/tablerepository.cc
    stage/src/fnordmetric/sql/runtime/tablescan.cc
    stage/src/fnordmetric/sql/svalue.cc
    stage/src/fnordmetric/sql_extensions/areachartbuilder.cc
    stage/src/fnordmetric/sql_extensions/barchartbuilder.cc
    stage/src/fnordmetric/sql_extensions/linechartbuilder.cc
    stage/src/fnordmetric/sql_extensions/pointchartbuilder.cc
    stage/src/fnordmetric/sql_extensions/domainconfig.cc
    stage/src/fnordmetric/sql_extensions/drawstatement.cc
    stage/src/fnordmetric/sql_extensions/seriesadapter.cc
    stage/src/fnordmetric/thread/threadpool.cc
    stage/src/fnordmetric/metricdb/adminui.cc
    stage/src/fnordmetric/metricdb/backends/disk/compactiontask.cc
    stage/src/fnordmetric/metricdb/backends/disk/metric.cc
    stage/src/fnordmetric/metricdb/backends/disk/metriccursor.cc
    stage/src/fnordmetric/metricdb/backends/disk/metricsnapshot.cc
    stage/src/fnordmetric/metricdb/backends/disk/metricrepository.cc
    stage/src/fnordmetric/metricdb/backends/disk/labelindex.cc
    stage/src/fnordmetric/metricdb/backends/disk/labelindexreader.cc
    stage/src/fnordmetric/metricdb/backends/disk/labelindexwriter.cc
    stage/src/fnordmetric/metricdb/backends/disk/samplereader.cc
    stage/src/fnordmetric/metricdb/backends/disk/samplewriter.cc
    stage/src/fnordmetric/metricdb/backends/disk/tableheaderreader.cc
    stage/src/fnordmetric/metricdb/backends/disk/tableheaderwriter.cc
    stage/src/fnordmetric/metricdb/backends/disk/tableref.cc
    stage/src/fnordmetric/metricdb/backends/disk/tokenindex.cc
    stage/src/fnordmetric/metricdb/backends/disk/tokenindexreader.cc
    stage/src/fnordmetric/metricdb/backends/disk/tokenindexwriter.cc
    stage/src/fnordmetric/metricdb/backends/inmemory/metric.cc
    stage/src/fnordmetric/metricdb/backends/inmemory/metricrepository.cc
    stage/src/fnordmetric/metricdb/backends/crate/metric.cc
    stage/src/fnordmetric/metricdb/backends/crate/metricrepository.cc
    stage/src/fnordmetric/metricdb/httpapi.cc
    stage/src/fnordmetric/metricdb/metric.cc
    stage/src/fnordmetric/metricdb/metricrepository.cc
    stage/src/fnordmetric/metricdb/metrictableref.cc
    stage/src/fnordmetric/metricdb/metrictablerepository.cc
    stage/src/fnordmetric/metricdb/sample.cc
    stage/src/fnordmetric/metricdb/statsd.cc)

include_directories(${PROJECT_BINARY_DIR})
include_directories(stage/src)
include_directories(stage/assets)

if(APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

add_executable(fnordmetric-cli ${FNORDMETRIC_SOURCES} stage/src/fnordmetric/cli.cc)
target_link_libraries(fnordmetric-cli m)
install(TARGETS fnordmetric-cli DESTINATION bin)

add_executable(fnordmetric-server ${FNORDMETRIC_SOURCES} stage/src/fnordmetric/server.cc)
target_link_libraries(fnordmetric-server m)
install(TARGETS fnordmetric-server DESTINATION bin)

find_package(Threads)
target_link_libraries(fnordmetric-cli ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(fnordmetric-server ${CMAKE_THREAD_LIBS_INIT})

find_package(MySQL)
if(MYSQL_FOUND)
  set(FNORD_ENABLE_MYSQL true)
  target_link_libraries(fnordmetric-cli ${MYSQL_CLIENT_LIBS})
  target_link_libraries(fnordmetric-server ${MYSQL_CLIENT_LIBS})
else()
  message("WARNING: libmysqlclient not found, FnordMetric will be compiled without MySQL support")
endif()

find_package(CURL)
if(CURL_FOUND)
  target_link_libraries(fnordmetric-cli ${CURL_LIBRARY})
  target_link_libraries(fnordmetric-server ${CURL_LIBRARY})
else()
    message("WARNING: libcurl not found, FnordMetric will be compiled without Crate support")
endif()

find_package(rapidjson)
if(RAPIDJSON_FOUND)
    target_link_libraries(fnordmetric-cli ${RAPIDJSON_LIBRARIES})
    target_link_libraries(fnordmetric-server ${RAPIDJSON_LIBRARIES})
else()
    message("WARNING: rapidjson not found, FnordMetric will be compiled without Crate support")
endif()

if(RAPIDJSON_FOUND AND CURL_FOUND)
    set(FNORD_ENABLE_CRATE true)
endif()

configure_file(config.h.in config.h)

if(ENABLE_TESTS)
  add_library(fnord SHARED ${FNORDMETRIC_SOURCES})
  target_link_libraries(fnord m)

  add_executable(tests/test-sql stage/src/fnordmetric/sql/sql_test.cc)
  target_link_libraries(tests/test-sql fnord)

  add_executable(tests/test-ui
       stage/src/fnordmetric/ui/ui_test.cc)
  target_link_libraries(tests/test-ui fnord)

  add_executable(tests/test-input-stream
      stage/src/fnordmetric/util/inputstream_test.cc)
  target_link_libraries(tests/test-input-stream fnord)

  add_executable(tests/test-uri
      stage/src/fnordmetric/util/uri_test.cc)
  target_link_libraries(tests/test-uri fnord)

  add_executable(tests/test-http
      stage/src/fnordmetric/http/http_test.cc)
  target_link_libraries(tests/test-http fnord)

  target_link_libraries(tests/test-uri fnord)
  add_executable(tests/test-fnv
      stage/src/fnordmetric/util/fnv_test.cc)
  target_link_libraries(tests/test-fnv fnord)

  add_executable(tests/test-csv-backend
      stage/src/fnordmetric/sql/backends/csv/csvbackend_test.cc)
  target_link_libraries(tests/test-csv-backend fnord)

  add_executable(tests/test-pagemanager
      stage/src/fnordmetric/io/pagemanager_test.cc)
  target_link_libraries(tests/test-pagemanager fnord)

  add_executable(tests/test-sstable
      stage/src/fnordmetric/sstable/sstable_test.cc)
  target_link_libraries(tests/test-sstable fnord)

  add_executable(tests/test-sql-extensions
      stage/src/fnordmetric/sql_extensions/sql_extensions_test.cc)
  target_link_libraries(tests/test-sql-extensions fnord)

  add_executable(tests/test-query
      stage/src/fnordmetric/query/query_test.cc)
  target_link_libraries(tests/test-query fnord)

  add_executable(tests/test-cli stage/src/fnordmetric/cli/cli_test.cc)
  target_link_libraries(tests/test-cli fnord)

  add_executable(tests/test-statsd
      stage/src/fnordmetric/metricdb/statsd_test.cc)
  target_link_libraries(tests/test-statsd fnord)

  add_executable(tests/test-disk-backend
      stage/src/fnordmetric/metricdb/backends/disk/diskbackend_test.cc)
  target_link_libraries(tests/test-disk-backend fnord)
endif()
