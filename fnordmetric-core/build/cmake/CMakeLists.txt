# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# FnordMetric is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3.0. You should have received a
# copy of the GNU General Public License along with this program. If not, see
# <http://www.gnu.org/licenses/>.
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules/") 
project(fnordmetric)

set(FNORDMETRIC_SOURCES
    stage/src/fnordmetric/cli/cli.cc
    stage/src/fnordmetric/cli/flagparser.cc
    stage/src/fnordmetric/cli/cli.cc
    stage/src/fnordmetric/io/file.cc
    stage/src/fnordmetric/io/fileutil.cc
    stage/src/fnordmetric/io/filerepository.cc
    stage/src/fnordmetric/io/mmappedfile.cc
    stage/src/fnordmetric/io/pagemanager.cc
    stage/src/fnordmetric/environment.cc
    stage/src/fnordmetric/ev/acceptor.cc
    stage/src/fnordmetric/ev/eventloop.cc
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
    stage/src/fnordmetric/thread/threadpool.cc
    stage/src/fnordmetric/web/queryendpoint.cc
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
    stage/src/fnordmetric/query/query.cc
    stage/src/fnordmetric/query/queryservice.cc
    stage/src/fnordmetric/sql/expressions/aggregate.cc
    stage/src/fnordmetric/sql/expressions/boolean.cc
    stage/src/fnordmetric/sql/expressions/datetime.cc
    stage/src/fnordmetric/sql/expressions/math.cc
    stage/src/fnordmetric/sql/parser/astnode.cc
    stage/src/fnordmetric/sql/parser/parser.cc
    stage/src/fnordmetric/sql/parser/token.cc
    stage/src/fnordmetric/sql/parser/tokenize.cc
    stage/src/fnordmetric/sql/runtime/compile.cc
    stage/src/fnordmetric/sql/runtime/defaultruntime.cc
    stage/src/fnordmetric/sql/runtime/execute.cc
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
    stage/src/fnordmetric/metricdb/adminui.cc
    stage/src/fnordmetric/metricdb/compactiontask.cc
    stage/src/fnordmetric/metricdb/httpapi.cc
    stage/src/fnordmetric/metricdb/labelindex.cc
    stage/src/fnordmetric/metricdb/labelindexreader.cc
    stage/src/fnordmetric/metricdb/labelindexwriter.cc
    stage/src/fnordmetric/metricdb/metric.cc
    stage/src/fnordmetric/metricdb/metriccursor.cc
    stage/src/fnordmetric/metricdb/metricrepository.cc
    stage/src/fnordmetric/metricdb/metricsnapshot.cc
    stage/src/fnordmetric/metricdb/metrictableref.cc
    stage/src/fnordmetric/metricdb/metrictablerepository.cc
    stage/src/fnordmetric/metricdb/samplereader.cc
    stage/src/fnordmetric/metricdb/samplewriter.cc
    stage/src/fnordmetric/metricdb/statsd.cc
    stage/src/fnordmetric/metricdb/tableheaderreader.cc
    stage/src/fnordmetric/metricdb/tableheaderwriter.cc
    stage/src/fnordmetric/metricdb/tableref.cc
    stage/src/fnordmetric/metricdb/tokenindex.cc
    stage/src/fnordmetric/metricdb/tokenindexreader.cc
    stage/src/fnordmetric/metricdb/tokenindexwriter.cc)

include_directories(stage/src)
include_directories(stage/assets)

IF(APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++")
ELSE()
  set(CMAKE_CXX_FLAGS "-std=c++0x")
ENDIF()

# libfnord
add_definitions("-Wno-predefined-identifier-outside-function")
add_library(fnord-static ${FNORDMETRIC_SOURCES})

find_package(MySQL)
if (MYSQL_FOUND)
  include_directories(${MYSQL_INCLUDE_DIRS})
  target_link_libraries(fnord-static mysqlclient)
endif(MYSQL_FOUND)

# fnordmetric
add_executable(fnordmetric-cli stage/src/fnordmetric/cli.cc)
target_link_libraries(fnordmetric-cli fnord-static)

add_executable(fnordmetric-server stage/src/fnordmetric/server.cc)
target_link_libraries(fnordmetric-server fnord-static)

# libfnord tests
add_executable(tests/test-sql stage/src/fnordmetric/sql/sql_test.cc)
target_link_libraries(tests/test-sql fnord-static)

add_executable(tests/test-ui
     stage/src/fnordmetric/ui/ui_test.cc)
target_link_libraries(tests/test-ui fnord-static)

add_executable(tests/test-input-stream
    stage/src/fnordmetric/util/inputstream_test.cc)
target_link_libraries(tests/test-input-stream fnord-static)

add_executable(tests/test-uri
    stage/src/fnordmetric/util/uri_test.cc)
target_link_libraries(tests/test-uri fnord-static)

add_executable(tests/test-csv-backend
    stage/src/fnordmetric/sql/backends/csv/csvbackend_test.cc)
target_link_libraries(tests/test-csv-backend fnord-static)

add_executable(tests/test-http stage/src/fnordmetric/http/http_test.cc)
target_link_libraries(tests/test-http fnord-static)

add_executable(tests/test-pagemanager
    stage/src/fnordmetric/io/pagemanager_test.cc)
target_link_libraries(tests/test-pagemanager fnord-static)

add_executable(tests/test-sstable
    stage/src/fnordmetric/sstable/sstable_test.cc)
target_link_libraries(tests/test-sstable fnord-static)

# fnordmetric tests
add_executable(tests/test-sql-extensions
    stage/src/fnordmetric/sql_extensions/sql_extensions_test.cc)
target_link_libraries(tests/test-sql-extensions fnord-static)

add_executable(tests/test-query
    stage/src/fnordmetric/query/query_test.cc)
target_link_libraries(tests/test-query fnord-static)

add_executable(tests/test-cli stage/src/fnordmetric/cli/cli_test.cc)
target_link_libraries(tests/test-cli fnord-static)

add_executable(tests/test-statsd
    stage/src/fnordmetric/metricdb/statsd_test.cc)
target_link_libraries(tests/test-statsd fnord-static)

add_executable(tests/test-metric
    stage/src/fnordmetric/metricdb/metric_test.cc)
target_link_libraries(tests/test-metric fnord-static)

add_executable(tests/test-metricrepository
    stage/src/fnordmetric/metricdb/metricrepository_test.cc)
target_link_libraries(tests/test-metricrepository fnord-static)
