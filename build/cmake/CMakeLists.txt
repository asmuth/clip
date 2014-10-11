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
    stage/src/fnordmetric/web/assets.cc
    stage/src/fnordmetric/web/queryendpoint.cc
    stage/src/fnordmetric/web/webinterface.cc
    stage/src/fnordmetric/environment.cc
    stage/src/fnordmetric/ev/acceptor.cc
    stage/src/fnordmetric/ev/eventloop.cc
    stage/src/fnordmetric/http/httpinputstream.cc
    stage/src/fnordmetric/http/httpoutputstream.cc
    stage/src/fnordmetric/http/httpmessage.cc
    stage/src/fnordmetric/http/httprequest.cc
    stage/src/fnordmetric/http/httpresponse.cc
    stage/src/fnordmetric/http/httpserver.cc
    stage/src/fnordmetric/util/datetime.cc
    stage/src/fnordmetric/util/exceptionhandler.cc
    stage/src/fnordmetric/util/format.cc
    stage/src/fnordmetric/util/inputstream.cc
    stage/src/fnordmetric/util/logger.cc
    stage/src/fnordmetric/util/logoutputstream.cc
    stage/src/fnordmetric/util/outputstream.cc
    stage/src/fnordmetric/util/jsonoutputstream.cc
    stage/src/fnordmetric/util/threadpool.cc
    stage/src/fnordmetric/util/runtimeexception.cc
    stage/src/fnordmetric/util/signalhandler.cc
    stage/src/fnordmetric/util/uri.cc
    stage/src/fnordmetric/util/wallclock.cc)

set(FNORDMETRIC_STORAGE_SOURCES
    stage/src/fnordmetric/storage/pagemanager.cc)

set(FNORDMETRIC_UI_SOURCES
    ${FNORDMETRIC_SOURCES}
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
    stage/src/fnordmetric/ui/timedomain.cc)

set(FNORDMETRIC_CSV_BACKEND_SOURCES
    stage/src/fnordmetric/sql/backends/csv/csvbackend.cc
    stage/src/fnordmetric/sql/backends/csv/csvinputstream.cc
    stage/src/fnordmetric/sql/backends/csv/csvtableref.cc)

set(FNORDMETRIC_MYSQL_BACKEND_SOURCES
    stage/src/fnordmetric/sql/backends/mysql/mysqlbackend.cc
    stage/src/fnordmetric/sql/backends/mysql/mysqlconnection.cc
    stage/src/fnordmetric/sql/backends/mysql/mysqltableref.cc)

set(FNORDMETRIC_QUERY_SOURCES
    ${FNORDMETRIC_UI_SOURCES}
    ${FNORDMETRIC_CSV_BACKEND_SOURCES}
    ${FNORDMETRIC_MYSQL_BACKEND_SOURCES}
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
    stage/src/fnordmetric/sql_extensions/seriesadapter.cc)

set(METRICDB_SOURCES
    stage/src/fnordmetric/metricdb/metrickey.cc
    stage/src/fnordmetric/metricdb/statsd.cc)

include_directories(stage/src)
include_directories(stage/assets)

IF(APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++")
ELSE()
  set(CMAKE_CXX_FLAGS "-std=c++0x")
ENDIF()

# libfnord
add_definitions("-Wno-predefined-identifier-outside-function")
add_library(fnord-static
    ${FNORDMETRIC_QUERY_SOURCES}
    ${FNORDMETRIC_STORAGE_SOURCES})

find_package(MySQL)
if (MYSQL_FOUND)
  include_directories(${MYSQL_INCLUDE_DIRS})
  target_link_libraries(fnord-static mysqlclient)
endif(MYSQL_FOUND)

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
    stage/src/fnordmetric/storage/pagemanager_test.cc)
target_link_libraries(tests/test-pagemanager fnord-static)

# chartsql
add_executable(chartsql stage/src/fnordmetric/main.cc)
target_link_libraries(chartsql fnord-static)

add_executable(tests/test-sql-extensions
    stage/src/fnordmetric/sql_extensions/sql_extensions_test.cc)
target_link_libraries(tests/test-sql-extensions fnord-static)

add_executable(tests/test-query
    stage/src/fnordmetric/query/query_test.cc)
target_link_libraries(tests/test-query fnord-static)

add_executable(tests/test-cli stage/src/fnordmetric/cli/cli_test.cc)
target_link_libraries(tests/test-cli fnord-static)

# metricdb
add_executable(metricdb stage/src/fnordmetric/metricdb/metricdb.cc)
target_link_libraries(metricdb fnord-static)

add_executable(tests/test-metrickey
    ${METRICDB_SOURCES}
    stage/src/fnordmetric/metricdb/metrickey_test.cc)
target_link_libraries(tests/test-metrickey fnord-static)

add_executable(tests/test-statsd
    ${METRICDB_SOURCES}
    stage/src/fnordmetric/metricdb/statsd_test.cc)
target_link_libraries(tests/test-statsd fnord-static)

