# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).
cmake_minimum_required(VERSION 2.6)
project(fnordmetric)

set(FNORDMETRIC_SOURCES
    stage/src/fnordmetric/util/format.cc
    stage/src/fnordmetric/util/inputstream.cc
    stage/src/fnordmetric/util/outputstream.cc
    stage/src/fnordmetric/util/runtimeexception.cc)

set(FNORDMETRIC_UI_SOURCES
    ${FNORDMETRIC_SOURCES}
    stage/src/fnordmetric/ui/areachart.cc
    stage/src/fnordmetric/ui/axisdefinition.cc
    stage/src/fnordmetric/ui/barchart.cc
    stage/src/fnordmetric/ui/canvas.cc
    stage/src/fnordmetric/ui/linechart.cc
    stage/src/fnordmetric/ui/pointchart.cc)

set(FNORDMETRIC_CSV_BACKEND_SOURCES
    stage/src/fnordmetric/query/backends/csv/csvbackend.cc
    stage/src/fnordmetric/query/backends/csv/csvinputstream.cc
    stage/src/fnordmetric/query/backends/csv/csvtableref.cc)

set(FNORDMETRIC_QUERY_SOURCES
    ${FNORDMETRIC_UI_SOURCES}
    ${FNORDMETRIC_CSV_BACKEND_SOURCES}
    stage/src/fnordmetric/query/astnode.cc
    stage/src/fnordmetric/query/compile.cc
    stage/src/fnordmetric/query/drawstatement.cc
    stage/src/fnordmetric/query/executable.cc
    stage/src/fnordmetric/query/execute.cc
    stage/src/fnordmetric/query/expressions/aggregate.cc
    stage/src/fnordmetric/query/expressions/boolean.cc
    stage/src/fnordmetric/query/expressions/math.cc
    stage/src/fnordmetric/query/grouper.cc
    stage/src/fnordmetric/query/parser.cc
    stage/src/fnordmetric/query/query.cc
    stage/src/fnordmetric/query/queryplan.cc
    stage/src/fnordmetric/query/queryservice.cc
    stage/src/fnordmetric/query/svalue.cc
    stage/src/fnordmetric/query/symboltable.cc
    stage/src/fnordmetric/query/token.cc
    stage/src/fnordmetric/query/tokenize.cc)

include_directories(stage/src)
set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++")

add_definitions("-Wno-predefined-identifier-outside-function")
# add_library(fnordmetric SHARED ${FNORDMETRIC_SOURCES})

add_executable(tests/test-query
    ${FNORDMETRIC_QUERY_SOURCES}
    stage/src/fnordmetric/query/query_test.cc)

add_executable(tests/test-ui
    ${FNORDMETRIC_UI_SOURCES}
    stage/src/fnordmetric/ui/ui_test.cc)

add_executable(tests/test-input-stream
    ${FNORDMETRIC_SOURCES}
    stage/src/fnordmetric/util/inputstream_test.cc)

add_executable(tests/test-csv-backend
    ${FNORDMETRIC_QUERY_SOURCES}
    stage/src/fnordmetric/query/backends/csv/csvbackend_test.cc)
