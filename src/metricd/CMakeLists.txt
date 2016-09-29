# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# FnordMetric is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3.0. You should have received a
# copy of the GNU General Public License along with this program. If not, see
# <http://www.gnu.org/licenses/>.
file(GLOB PROTO_FILES *.proto)
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

add_library(fnord-metricdb OBJECT
    backends/disk/compactiontask.cc
    backends/disk/labelindex.cc
    backends/disk/labelindexreader.cc
    backends/disk/labelindexwriter.cc
    backends/disk/metric.cc
    backends/disk/metriccursor.cc
    backends/disk/metricrepository.cc
    backends/disk/metricsnapshot.cc
    backends/disk/samplereader.cc
    backends/disk/samplewriter.cc
    backends/disk/tableheaderreader.cc
    backends/disk/tableheaderwriter.cc
    backends/disk/tableref.cc
    backends/disk/tokenindex.cc
    backends/disk/tokenindexreader.cc
    backends/disk/tokenindexwriter.cc
    backends/inmemory/metric.cc
    backends/inmemory/metricrepository.cc
    backends/tsdb/metric.cc
    backends/tsdb/metricrepository.cc
    httpapiservlet.cc
    metric.cc
    metricrepository.cc
    metricservice.cc
    timeseriesquery.cc
    sample.cc
    ${PROTO_SRCS})

add_executable(metricd
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-chart>
    $<TARGET_OBJECTS:fnord-metricdb>
    $<TARGET_OBJECTS:fnord-http>
    $<TARGET_OBJECTS:fnord-json>
    $<TARGET_OBJECTS:sstable>
    $<TARGET_OBJECTS:tsdb>
    $<TARGET_OBJECTS:fnord-mdb>
    $<TARGET_OBJECTS:fnord-protobuf>
    $<TARGET_OBJECTS:cstable>
    $<TARGET_OBJECTS:dproc>
    $<TARGET_OBJECTS:sensord-client>
    metricd.cc)

target_link_libraries(metricd ${CMAKE_THREAD_LIBS_INIT} ${FNORD_LIBS})

