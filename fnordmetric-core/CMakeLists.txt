# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# Licensed under the MIT license (see LICENSE).
cmake_minimum_required(VERSION 2.6)

set(FNORDMETRIC_SOURCES
        query.cc
        query/astnode.cc
        query/compile.cc
        query/executable.cc
        query/execute.cc
        query/grouper.cc
        query/parser.cc
        query/queryplan.cc
        query/svalue.cc
        query/symboltable.cc
        query/token.cc
        query/tokenize.cc
        query/expressions/aggregate.cc
        query/expressions/boolean.cc
        query/expressions/math.cc
        drawables/barchart.cc
        format.cc)

include_directories(./)
set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++")

add_library(fnordmetric SHARED ${FNORDMETRIC_SOURCES})

add_executable(query-test query_test.cc)
target_link_libraries(query-test fnordmetric)
add_test(QueryTest query-test)

