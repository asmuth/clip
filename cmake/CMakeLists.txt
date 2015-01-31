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

include_directories(${PROJECT_BINARY_DIR})
include_directories(../3rdparty/libfnord)
include_directories(../src)

add_subdirectory(../3rdparty/libfnord/fnord-base "${CMAKE_CURRENT_BINARY_DIR}/libfnord-base")
add_subdirectory(../3rdparty/libfnord/fnord-chart "${CMAKE_CURRENT_BINARY_DIR}/libfnord-chart")
add_subdirectory(../3rdparty/libfnord/fnord-http "${CMAKE_CURRENT_BINARY_DIR}/libfnord-http")
add_subdirectory(../3rdparty/libfnord/fnord-json "${CMAKE_CURRENT_BINARY_DIR}/libfnord-json")
add_subdirectory(../3rdparty/libfnord/fnord-metricdb "${CMAKE_CURRENT_BINARY_DIR}/libfnord-metricdb")
add_subdirectory(../3rdparty/libfnord/fnord-sstable "${CMAKE_CURRENT_BINARY_DIR}/libfnord-sstable")

find_package(Threads)

if(APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

set(FNORDMETRIC_SOURCES
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-chart>
    $<TARGET_OBJECTS:fnord-http>
    $<TARGET_OBJECTS:fnord-json>
    $<TARGET_OBJECTS:fnord-metricdb>
    $<TARGET_OBJECTS:fnord-sstable>
    ../src/environment.cc)
    #stage/src/fnordmetric/util/assets.cc
    #stage/src/fnordmetric/sql/backends/csv/csvbackend.cc
    #stage/src/fnordmetric/sql/backends/csv/csvinputstream.cc
    #stage/src/fnordmetric/sql/backends/csv/csvtableref.cc
    #stage/src/fnordmetric/sql/backends/mysql/mysqlbackend.cc
    #stage/src/fnordmetric/sql/backends/mysql/mysqlconnection.cc
    #stage/src/fnordmetric/sql/backends/mysql/mysqltableref.cc
    #stage/src/fnordmetric/sql/expressions/aggregate.cc
    #stage/src/fnordmetric/sql/expressions/boolean.cc
    #stage/src/fnordmetric/sql/expressions/datetime.cc
    #stage/src/fnordmetric/sql/expressions/math.cc
    #stage/src/fnordmetric/sql/parser/astnode.cc
    #stage/src/fnordmetric/sql/parser/astutil.cc
    #stage/src/fnordmetric/sql/parser/parser.cc
    #stage/src/fnordmetric/sql/parser/token.cc
    #stage/src/fnordmetric/sql/parser/tokenize.cc
    #stage/src/fnordmetric/sql/runtime/compile.cc
    #stage/src/fnordmetric/sql/runtime/defaultruntime.cc
    #stage/src/fnordmetric/sql/runtime/execute.cc
    #stage/src/fnordmetric/sql/runtime/groupovertimewindow.cc
    #stage/src/fnordmetric/sql/runtime/orderby.cc
    #stage/src/fnordmetric/sql/runtime/importstatement.cc
    #stage/src/fnordmetric/sql/runtime/queryplan.cc
    #stage/src/fnordmetric/sql/runtime/queryplanbuilder.cc
    #stage/src/fnordmetric/sql/runtime/queryplannode.cc
    #stage/src/fnordmetric/sql/runtime/runtime.cc
    #stage/src/fnordmetric/sql/runtime/symboltable.cc
    #stage/src/fnordmetric/sql/runtime/tablerepository.cc
    #stage/src/fnordmetric/sql/runtime/tablescan.cc
    #stage/src/fnordmetric/sql/svalue.cc
    #stage/src/fnordmetric/sql_extensions/areachartbuilder.cc
    #stage/src/fnordmetric/sql_extensions/barchartbuilder.cc
    #stage/src/fnordmetric/sql_extensions/linechartbuilder.cc
    #stage/src/fnordmetric/sql_extensions/pointchartbuilder.cc
    #stage/src/fnordmetric/sql_extensions/domainconfig.cc
    #stage/src/fnordmetric/sql_extensions/drawstatement.cc
    #stage/src/fnordmetric/sql_extensions/seriesadapter.cc
    #stage/src/fnordmetric/query/query.cc
    #stage/src/fnordmetric/query/queryservice.cc

#add_executable(fnordmetric-cli ${FNORDMETRIC_SOURCES} ../src/cli.cc)
#target_link_libraries(fnordmetric-cli m)
#target_link_libraries(fnordmetric-cli ${CMAKE_THREAD_LIBS_INIT})
#install(TARGETS fnordmetric-cli DESTINATION bin)

add_executable(fnordmetric-server ${FNORDMETRIC_SOURCES} ../src/fnordmetric-server.cc)
target_link_libraries(fnordmetric-server m)
target_link_libraries(fnordmetric-server ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS fnordmetric-server DESTINATION bin)

find_package(MySQL)
if(MYSQL_FOUND)
  set(FNORD_ENABLE_MYSQL true)
  #target_link_libraries(fnordmetric-cli ${MYSQL_CLIENT_LIBS})
  target_link_libraries(fnordmetric-server ${MYSQL_CLIENT_LIBS})
else()
  message("WARNING: libmysqlclient not found, FnordMetric will be compiled without MySQL support")
endif()

configure_file(config.h.in config.h)

if(ENABLE_TESTS)
  add_library(fnord SHARED ${FNORDMETRIC_SOURCES})
  target_link_libraries(fnord m)

  add_executable(tests/test-sql ../src/sql/sql_test.cc)
  target_link_libraries(tests/test-sql fnord)

  add_executable(tests/test-csv-backend
      stage/src/fnordmetric/sql/backends/csv/csvbackend_test.cc)
  target_link_libraries(tests/test-csv-backend fnord)

  add_executable(tests/test-sql-extensions
      stage/src/fnordmetric/sql_extensions/sql_extensions_test.cc)
  target_link_libraries(tests/test-sql-extensions fnord)

  add_executable(tests/test-query
      stage/src/fnordmetric/query/query_test.cc)
  target_link_libraries(tests/test-query fnord)

  add_executable(tests/test-disk-backend
      stage/src/fnordmetric/metricdb/backends/disk/diskbackend_test.cc)
  target_link_libraries(tests/test-disk-backend fnord)
endif()
