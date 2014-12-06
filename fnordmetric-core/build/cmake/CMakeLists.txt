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
    stage/src/fnord/base/assets.cc
    stage/src/fnord/base/buffer.cc
    stage/src/fnord/base/datetime.cc
    stage/src/fnord/base/exceptionhandler.cc
    stage/src/fnord/base/ieee754.cc
    stage/src/fnord/base/inspect.cc
    stage/src/fnord/base/random.cc
    stage/src/fnord/base/exception.cc
    stage/src/fnord/base/stringutil.cc
    stage/src/fnord/base/uri.cc
    stage/src/fnord/base/wallclock.cc
    stage/src/fnord/chart/axisdefinition.cc
    stage/src/fnord/chart/areachart.cc
    stage/src/fnord/chart/barchart.cc
    stage/src/fnord/chart/linechart.cc
    stage/src/fnord/chart/pointchart.cc
    stage/src/fnord/chart/canvas.cc
    stage/src/fnord/chart/domain.cc
    stage/src/fnord/chart/domainprovider.cc
    stage/src/fnord/chart/drawable.cc
    stage/src/fnord/chart/griddefinition.cc
    stage/src/fnord/chart/legenddefinition.cc
    stage/src/fnord/chart/series.cc
    stage/src/fnord/chart/timedomain.cc
    stage/src/fnord/hash/fnv.cc
    stage/src/fnord/net/http/httpinputstream.cc
    stage/src/fnord/net/http/httpoutputstream.cc
    stage/src/fnord/net/http/httpmessage.cc
    stage/src/fnord/net/http/httprequest.cc
    stage/src/fnord/net/http/httpresponse.cc
    stage/src/fnord/net/http/httpserver.cc
    stage/src/fnord/io/file.cc
    stage/src/fnord/io/fileutil.cc
    stage/src/fnord/io/filerepository.cc
    stage/src/fnord/io/mmappedfile.cc
    stage/src/fnord/io/pagemanager.cc
    stage/src/fnord/io/inputstream.cc
    stage/src/fnord/io/outputstream.cc
    stage/src/fnord/json/flatjsonreader.cc
    stage/src/fnord/json/jsondocument.cc
    stage/src/fnord/json/jsoninputstream.cc
    stage/src/fnord/json/jsonoutputstream.cc
    stage/src/fnord/json/jsonpointer.cc
    stage/src/fnord/json/jsonrpc.cc
    stage/src/fnord/json/jsonrpchttpadapter.cc
    stage/src/fnord/json/jsonrpcrequest.cc
    stage/src/fnord/json/jsonrpcresponse.cc
    stage/src/fnord/net/udpserver.cc
    stage/src/fnord/net/statsd/statsd.cc
    stage/src/fnord/service/metric/backends/disk/compactiontask.cc
    stage/src/fnord/service/metric/backends/disk/metric.cc
    stage/src/fnord/service/metric/backends/disk/metriccursor.cc
    stage/src/fnord/service/metric/backends/disk/metricsnapshot.cc
    stage/src/fnord/service/metric/backends/disk/metricrepository.cc
    stage/src/fnord/service/metric/backends/disk/labelindex.cc
    stage/src/fnord/service/metric/backends/disk/labelindexreader.cc
    stage/src/fnord/service/metric/backends/disk/labelindexwriter.cc
    stage/src/fnord/service/metric/backends/disk/samplereader.cc
    stage/src/fnord/service/metric/backends/disk/samplewriter.cc
    stage/src/fnord/service/metric/backends/disk/tableheaderreader.cc
    stage/src/fnord/service/metric/backends/disk/tableheaderwriter.cc
    stage/src/fnord/service/metric/backends/disk/tableref.cc
    stage/src/fnord/service/metric/backends/disk/tokenindex.cc
    stage/src/fnord/service/metric/backends/disk/tokenindexreader.cc
    stage/src/fnord/service/metric/backends/disk/tokenindexwriter.cc
    stage/src/fnord/service/metric/backends/inmemory/metric.cc
    stage/src/fnord/service/metric/backends/inmemory/metricrepository.cc
    stage/src/fnord/service/metric/metric.cc
    stage/src/fnord/service/metric/metricrepository.cc
    stage/src/fnord/service/metric/metricservice.cc
    stage/src/fnord/service/metric/metricserviceadapter.cc
    stage/src/fnord/service/metric/sample.cc
    stage/src/fnord/sstable/cursor.cc
    stage/src/fnord/sstable/fileheaderreader.cc
    stage/src/fnord/sstable/fileheaderwriter.cc
    stage/src/fnord/sstable/index.cc
    stage/src/fnord/sstable/indexprovider.cc
    stage/src/fnord/sstable/rowoffsetindex.cc
    stage/src/fnord/sstable/sstablereader.cc
    stage/src/fnord/sstable/sstablerepair.cc
    stage/src/fnord/sstable/sstablewriter.cc
    stage/src/fnord/system/signalhandler.cc
    stage/src/fnord/thread/threadpool.cc
    stage/src/fnord/util/binarymessagereader.cc
    stage/src/fnord/util/binarymessagewriter.cc
    stage/src/fnord/webui/bundle.cc
    stage/src/fnord/webui/httpmount.cc
    stage/src/fnordmetric/chartsql/query.cc
    stage/src/fnordmetric/chartsql/queryservice.cc
    stage/src/fnordmetric/chartsql/areachartbuilder.cc
    stage/src/fnordmetric/chartsql/barchartbuilder.cc
    stage/src/fnordmetric/chartsql/linechartbuilder.cc
    stage/src/fnordmetric/chartsql/pointchartbuilder.cc
    stage/src/fnordmetric/chartsql/domainconfig.cc
    stage/src/fnordmetric/chartsql/drawstatement.cc
    stage/src/fnordmetric/chartsql/seriesadapter.cc
    stage/src/fnordmetric/cli/cli.cc
    stage/src/fnordmetric/cli/flagparser.cc
    stage/src/fnordmetric/environment.cc
    stage/src/fnordmetric/sql/backends/csv/csvbackend.cc
    stage/src/fnordmetric/sql/backends/csv/csvinputstream.cc
    stage/src/fnordmetric/sql/backends/csv/csvtableref.cc
    stage/src/fnordmetric/sql/backends/metricservice/metrictableref.cc
    stage/src/fnordmetric/sql/backends/metricservice/metrictablerepository.cc
    stage/src/fnordmetric/sql/backends/mysql/mysqlbackend.cc
    stage/src/fnordmetric/sql/backends/mysql/mysqlconnection.cc
    stage/src/fnordmetric/sql/backends/mysql/mysqltableref.cc
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
    stage/src/fnordmetric/util/logger.cc
    stage/src/fnordmetric/util/logoutputstream.cc
    stage/src/fnordmetric/adminui.cc
    stage/src/fnordmetric/httpapi.cc)

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

configure_file(config.h.in config.h)

if(ENABLE_TESTS)
  add_library(fnord SHARED ${FNORDMETRIC_SOURCES})
  target_link_libraries(fnord m)

  add_executable(tests/test-sql stage/src/fnordmetric/sql/sql_test.cc)
  target_link_libraries(tests/test-sql fnord)

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
