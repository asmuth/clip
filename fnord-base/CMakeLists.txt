# This file is part of the "FnordMetric" project
#   Copyright (c) 2014 Paul Asmuth, Google Inc.
#
# FnordMetric is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License v3.0. You should have received a
# copy of the GNU General Public License along with this program. If not, see
# <http://www.gnu.org/licenses/>.
cmake_minimum_required(VERSION 2.8.8)
project(libfnord-base)
include(FindPkgConfig)

option(ENABLE_TESTS "Build unit tests [default: ON]" ON)

include_directories(${PROJECT_BINARY_DIR})
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/tests)
include_directories(../)
include_directories(../3rdparty/)

find_package(Threads)

if(APPLE)
  set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

# base
add_library(fnord-base OBJECT
    application.cc
    autoref.cc
    assets.cc
    buffer.cc
    bufferutil.cc
    cli/flagparser.cc
    datetime.cc
    duration.cc
    exception.cc
    exceptionhandler.cc
    fnv.cc
    ieee754.cc
    inspect.cc
    io/file.cc
    io/fileutil.cc
    io/filerepository.cc
    io/inputstream.cc
    io/outputstream.cc
    io/mmappedfile.cc
    io/pagemanager.cc
    logging.cc
    logging/logoutputstream.cc
    net/dnscache.cc
    net/tcpserver.cc
    net/udpserver.cc
    net/udpsocket.cc
    net/inetaddr.cc
    net/tcpconnection.cc
    random.cc
    status.cc
    stringutil.cc
    stats/statsdagent.cc
    stats/statsrepository.cc
    stats/statssink.cc
    thread/eventloop.cc
    thread/signalhandler.cc
    thread/threadpool.cc
    thread/wakeup.cc
    uri.cc
    util/binarymessagereader.cc
    util/binarymessagewriter.cc
    wallclock.cc)

add_executable(tests/test-inputstream
    $<TARGET_OBJECTS:fnord-base>
    io/inputstream_test.cc)

add_executable(tests/test-uri
    $<TARGET_OBJECTS:fnord-base>
    uri_test.cc)

add_executable(tests/test-stringutil
    $<TARGET_OBJECTS:fnord-base>
    stringutil_test.cc)

## redis
#add_library(fnord-redis OBJECT
#    ../3rdparty/hiredis/async.c
#    ../3rdparty/hiredis/dict.c
#    ../3rdparty/hiredis/hiredis.c
#    ../3rdparty/hiredis/net.c
#    ../3rdparty/hiredis/sds.c
#    net/redis/redisconnection.cc
#    net/redis/redisqueue.cc)

#add_executable(tests/test-redis
#    $<TARGET_OBJECTS:fnord-base>
#    $<TARGET_OBJECTS:fnord-comm>
#    $<TARGET_OBJECTS:fnord-net>
#    $<TARGET_OBJECTS:fnord-redis>
#    net/redis/redis_test.cc)
