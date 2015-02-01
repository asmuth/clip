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
include_directories(../src)
include_directories(../src/libfnord)

add_subdirectory(../src/libfnord/fnord-base "${CMAKE_CURRENT_BINARY_DIR}/libfnord-base")
add_subdirectory(../src/libfnord/fnord-chart "${CMAKE_CURRENT_BINARY_DIR}/libfnord-chart")
add_subdirectory(../src/libfnord/fnord-http "${CMAKE_CURRENT_BINARY_DIR}/libfnord-http")
add_subdirectory(../src/libfnord/fnord-json "${CMAKE_CURRENT_BINARY_DIR}/libfnord-json")
add_subdirectory(../src/libfnord/fnord-metricdb "${CMAKE_CURRENT_BINARY_DIR}/libfnord-metricdb")
add_subdirectory(../src/libfnord/fnord-sstable "${CMAKE_CURRENT_BINARY_DIR}/libfnord-sstable")

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
    ../src/libfnord/fnord-webcomponents/bundle.cc
    ../src/libfnord/fnord-webcomponents/httpmount.cc
    ../src/fnordmetric/adminui.cc
    ../src/fnordmetric/environment.cc
    ../src/fnordmetric/chartsql/areachartbuilder.cc
    ../src/fnordmetric/chartsql/barchartbuilder.cc
    ../src/fnordmetric/chartsql/domainconfig.cc
    ../src/fnordmetric/chartsql/drawstatement.cc
    ../src/fnordmetric/chartsql/linechartbuilder.cc
    ../src/fnordmetric/chartsql/pointchartbuilder.cc
    ../src/fnordmetric/chartsql/queryendpoint.cc
    ../src/fnordmetric/chartsql/query.cc
    ../src/fnordmetric/chartsql/queryservice.cc
    ../src/fnordmetric/chartsql/seriesadapter.cc
    ../src/fnordmetric/sql/backends/csv/csvbackend.cc
    ../src/fnordmetric/sql/backends/csv/csvinputstream.cc
    ../src/fnordmetric/sql/backends/csv/csvtableref.cc
    ../src/fnordmetric/sql/backends/mysql/mysqlbackend.cc
    ../src/fnordmetric/sql/backends/mysql/mysqlconnection.cc
    ../src/fnordmetric/sql/backends/mysql/mysqltableref.cc
    ../src/fnordmetric/sql/backends/metricservice/metrictableref.cc
    ../src/fnordmetric/sql/backends/metricservice/metrictablerepository.cc
    ../src/fnordmetric/sql/expressions/aggregate.cc
    ../src/fnordmetric/sql/expressions/boolean.cc
    ../src/fnordmetric/sql/expressions/datetime.cc
    ../src/fnordmetric/sql/expressions/math.cc
    ../src/fnordmetric/sql/parser/astnode.cc
    ../src/fnordmetric/sql/parser/astutil.cc
    ../src/fnordmetric/sql/parser/parser.cc
    ../src/fnordmetric/sql/parser/token.cc
    ../src/fnordmetric/sql/parser/tokenize.cc
    ../src/fnordmetric/sql/runtime/compile.cc
    ../src/fnordmetric/sql/runtime/defaultruntime.cc
    ../src/fnordmetric/sql/runtime/execute.cc
    ../src/fnordmetric/sql/runtime/groupovertimewindow.cc
    ../src/fnordmetric/sql/runtime/orderby.cc
    ../src/fnordmetric/sql/runtime/importstatement.cc
    ../src/fnordmetric/sql/runtime/queryplan.cc
    ../src/fnordmetric/sql/runtime/queryplanbuilder.cc
    ../src/fnordmetric/sql/runtime/queryplannode.cc
    ../src/fnordmetric/sql/runtime/runtime.cc
    ../src/fnordmetric/sql/runtime/symboltable.cc
    ../src/fnordmetric/sql/runtime/tablerepository.cc
    ../src/fnordmetric/sql/runtime/tablescan.cc
    ../src/fnordmetric/sql/svalue.cc)


set_source_files_properties(assets PROPERTIES GENERATED true)
add_custom_target(fnordmetric-assets COMMAND ASSETS_FILE=${CMAKE_BINARY_DIR}/fnordmetric-assets.cc ./assets.sh WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

#add_executable(fnordmetric-cli ${FNORDMETRIC_SOURCES} ../src/fnordmetric/cli.cc)
#target_link_libraries(fnordmetric-cli m)
#target_link_libraries(fnordmetric-cli ${CMAKE_THREAD_LIBS_INIT})
#install(TARGETS fnordmetric-cli DESTINATION bin)

add_executable(fnordmetric-server
    ${FNORDMETRIC_SOURCES}
    ../src/fnordmetric/fnordmetric-server.cc
    ${CMAKE_BINARY_DIR}/fnordmetric-assets.cc)
add_dependencies(fnordmetric-server fnordmetric-assets)
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

  add_executable(tests/test-sql ../src/fnordmetric/sql/sql_test.cc)
  target_link_libraries(tests/test-sql fnord)

  add_executable(tests/test-csv-backend
      stage/src/fnordmetric/fnordmetric/sql/backends/csv/csvbackend_test.cc)
  target_link_libraries(tests/test-csv-backend fnord)

  add_executable(tests/test-sql-extensions
      stage/src/fnordmetric/fnordmetric/sql_extensions/sql_extensions_test.cc)
  target_link_libraries(tests/test-sql-extensions fnord)

  add_executable(tests/test-query
      stage/src/fnordmetric/fnordmetric/query/query_test.cc)
  target_link_libraries(tests/test-query fnord)

  add_executable(tests/test-disk-backend
      stage/src/fnordmetric/fnordmetric/metricdb/backends/disk/diskbackend_test.cc)
  target_link_libraries(tests/test-disk-backend fnord)
endif()
