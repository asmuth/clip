# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).
cmake_minimum_required(VERSION 2.6)
project(fnordmetric)
enable_testing()

set(FNORDMETRIC_SOURCES
    src/util/format.cc
    src/util/runtimeexception.cc)

set(FNORDMETRIC_UI_SOURCES
    ${FNORDMETRIC_SOURCES}
    src/ui/areachart.cc
    src/ui/axisdefinition.cc
    src/ui/barchart.cc
    src/ui/canvas.cc
    src/ui/linechart.cc
    src/ui/pointchart.cc)

set(FNORDMETRIC_QUERY_SOURCES
    ${FNORDMETRIC_UI_SOURCES}
    src/query/query.cc
    src/query/astnode.cc
    src/query/compile.cc
    src/query/drawstatement.cc
    src/query/executable.cc
    src/query/execute.cc
    src/query/expressions/aggregate.cc
    src/query/expressions/boolean.cc
    src/query/expressions/math.cc
    src/query/grouper.cc
    src/query/parser.cc
    src/query/queryplan.cc
    src/query/svalue.cc
    src/query/symboltable.cc
    src/query/token.cc
    src/query/tokenize.cc)

set(FNORDMETRIC_CSV_BACKEND_SOURCES
    src/backends/csv/csvfile.cc)

include_directories(./src)
set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++")

# add_library(fnordmetric SHARED ${FNORDMETRIC_SOURCES})

add_test(QueryTest test/test-query)
add_executable(test/test-query
    ${FNORDMETRIC_QUERY_SOURCES}
    src/query/query_test.cc)

add_test(UITest test/test-ui)
add_executable(test/test-ui
    ${FNORDMETRIC_UI_SOURCES}
    src/ui/ui_test.cc)

add_test(CSVBackendTest test/test-csv-backend)
add_executable(test/test-csv-backend
    ${FNORDMETRIC_UI_SOURCES}
    ${FNORDMETRIC_CSV_BACKEND_SOURCES}
    src/backends/csv/csvbackend_test.cc)
