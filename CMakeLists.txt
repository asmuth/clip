# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).
cmake_minimum_required(VERSION 2.6)
project(fnordmetric)
enable_testing()

set(FNORDMETRIC_SOURCES)

set(FNORDMETRIC_QUERY_SOURCES
    ${FNORDMETRIC_SOURCES}
    src/query/query.cc
    src/query/astnode.cc
    src/query/compile.cc
    src/query/executable.cc
    src/query/execute.cc
    src/query/grouper.cc
    src/query/parser.cc
    src/query/queryplan.cc
    src/query/svalue.cc
    src/query/symboltable.cc
    src/query/token.cc
    src/query/tokenize.cc
    src/query/expressions/aggregate.cc
    src/query/expressions/boolean.cc
    src/query/expressions/math.cc)

set(FNORDMETRIC_UI_SOURCES
    ${FNORDMETRIC_SOURCES}
    src/ui/canvas.cc)

include_directories(./src)
set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++")

# add_library(fnordmetric SHARED ${FNORDMETRIC_SOURCES})

add_test(QueryTest test/query-test)
add_executable(test/query-test
    ${FNORDMETRIC_QUERY_SOURCES}
    src/query/query_test.cc)


add_test(QueryTest test/ui-test)
add_executable(test/ui-test
    ${FNORDMETRIC_UI_SOURCES}
    src/ui/ui_test.cc)
