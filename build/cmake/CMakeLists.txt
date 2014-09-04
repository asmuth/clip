# This file is part of the "FnordMetric" project
#   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
#
# FnordMetric is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3.0. You should have received a
# copy of the GNU General Public License along with this program. If not, see
# <http://www.gnu.org/licenses/>.
cmake_minimum_required(VERSION 2.6)
project(fnordmetric)

set(FNORDMETRIC_SOURCES
    stage/src/fnordmetric/base/series.cc
    stage/src/fnordmetric/ev/acceptor.cc
    stage/src/fnordmetric/ev/eventloop.cc
    stage/src/fnordmetric/http/httpinputstream.cc
    stage/src/fnordmetric/http/httpoutputstream.cc
    stage/src/fnordmetric/http/httpmessage.cc
    stage/src/fnordmetric/http/httprequest.cc
    stage/src/fnordmetric/http/httpresponse.cc
    stage/src/fnordmetric/http/httpserver.cc
    stage/src/fnordmetric/util/exceptionhandler.cc
    stage/src/fnordmetric/util/format.cc
    stage/src/fnordmetric/util/inputstream.cc
    stage/src/fnordmetric/util/outputstream.cc
    stage/src/fnordmetric/util/jsonoutputstream.cc
    stage/src/fnordmetric/util/threadpool.cc
    stage/src/fnordmetric/util/runtimeexception.cc)

#stage/src/fnordmetric/ui/areachart.cc
#stage/src/fnordmetric/ui/linechart.cc
#stage/src/fnordmetric/ui/pointchart.cc)

set(FNORDMETRIC_UI_SOURCES
    ${FNORDMETRIC_SOURCES}
    stage/src/fnordmetric/ui/axisdefinition.cc
    stage/src/fnordmetric/ui/barchart.cc
    stage/src/fnordmetric/ui/linechart.cc
    stage/src/fnordmetric/ui/canvas.cc
    stage/src/fnordmetric/ui/domain.cc)

set(FNORDMETRIC_CSV_BACKEND_SOURCES
    stage/src/fnordmetric/sql/backends/csv/csvbackend.cc
    stage/src/fnordmetric/sql/backends/csv/csvinputstream.cc
    stage/src/fnordmetric/sql/backends/csv/csvtableref.cc)

set(FNORDMETRIC_QUERY_SOURCES
    ${FNORDMETRIC_UI_SOURCES}
    ${FNORDMETRIC_CSV_BACKEND_SOURCES}
    stage/src/fnordmetric/query/query.cc
    stage/src/fnordmetric/query/queryservice.cc
    stage/src/fnordmetric/sql/astnode.cc
    stage/src/fnordmetric/sql/compile.cc
    stage/src/fnordmetric/sql/execute.cc
    stage/src/fnordmetric/sql/grouper.cc
    stage/src/fnordmetric/sql/parser.cc
    stage/src/fnordmetric/sql/queryplanbuilder.cc
    stage/src/fnordmetric/sql/queryplannode.cc
    stage/src/fnordmetric/sql/svalue.cc
    stage/src/fnordmetric/sql/symboltable.cc
    stage/src/fnordmetric/sql/token.cc
    stage/src/fnordmetric/sql/tokenize.cc
    stage/src/fnordmetric/sql_extensions/barchartbuilder.cc
    stage/src/fnordmetric/sql_extensions/linechartbuilder.cc
    stage/src/fnordmetric/sql_extensions/drawstatement.cc
    stage/src/fnordmetric/sql_extensions/seriesadapter.cc)

set(FNORDMETRIC_CLI_SOURCES
    stage/src/fnordmetric/cli/cli.cc
    stage/src/fnordmetric/cli/flagparser.cc
    stage/src/fnordmetric/web/assets.cc
    stage/src/fnordmetric/web/queryendpoint.cc
    stage/src/fnordmetric/web/webinterface.cc
    stage/src/fnordmetric/sql/expressions/aggregate.cc
    stage/src/fnordmetric/sql/expressions/boolean.cc
    stage/src/fnordmetric/sql/expressions/math.cc
    stage/src/fnordmetric/main.cc)

include_directories(stage/src)
include_directories(stage/assets)
#set(CMAKE_CXX_FLAGS "-std=c++0x")
set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++")

add_definitions("-Wno-predefined-identifier-outside-function")
# add_library(fnordmetric SHARED ${FNORDMETRIC_SOURCES})

add_library(libfnordmetric ${FNORDMETRIC_QUERY_SOURCES})

add_executable(fnordmetric ${FNORDMETRIC_CLI_SOURCES})
target_link_libraries(fnordmetric libfnordmetric)

add_executable(tests/test-sql
    stage/src/fnordmetric/sql/sql_test.cc
    stage/src/fnordmetric/sql/expressions/aggregate.cc
    stage/src/fnordmetric/sql/expressions/boolean.cc
    stage/src/fnordmetric/sql/expressions/math.cc)
target_link_libraries(tests/test-sql libfnordmetric)

# add_executable(tests/test-query
#    ${FNORDMETRIC_QUERY_SOURCES}
#    stage/src/fnordmetric/query/query_test.cc)

# add_executable(tests/test-ui
#     ${FNORDMETRIC_UI_SOURCES}
#     stage/src/fnordmetric/ui/ui_test.cc)
# 
# add_executable(tests/test-cli
#     ${FNORDMETRIC_CLI_SOURCES}
#     stage/src/fnordmetric/cli/cli_test.cc)
# 

add_executable(tests/test-input-stream
    stage/src/fnordmetric/util/inputstream_test.cc)
target_link_libraries(tests/test-input-stream libfnordmetric)

add_executable(tests/test-csv-backend
    stage/src/fnordmetric/sql/backends/csv/csvbackend_test.cc)
target_link_libraries(tests/test-csv-backend libfnordmetric)

add_executable(tests/test-http stage/src/fnordmetric/http/http_test.cc)
target_link_libraries(tests/test-http libfnordmetric)

