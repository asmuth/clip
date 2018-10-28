cmake_minimum_required(VERSION 2.8.8)
project(signaltk)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/extra/cmake")
set(CMAKE_CXX_STANDARD 11)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(Threads)
find_package(Cairo)
include_directories(${CAIRO_INCLUDE_DIRS})

add_definitions(-DFNORDMETRIC_VERSION="unstable")

add_executable(signaltk
    src/signaltk/core/path.cc
    src/signaltk/core/brush.cc
    src/signaltk/core/colour.cc
    src/signaltk/core/layer.cc
    src/signaltk/core/text.cc
    src/signaltk/core/image_api.cc
    src/signaltk/plot/gridlines.cc
    src/signaltk/plot/axes.cc
    src/signaltk/plot/domain.cc
    src/signaltk/plot/domainprovider.cc
    src/signaltk/plot/legenddefinition.cc
    src/signaltk/plot/series.cc
    src/signaltk/plot/timedomain.cc
    src/signaltk/plot/linechart.cc
    src/signaltk/plot/plot_api.cc
    src/signaltk/util/random.cc
    src/signaltk/util/bufferutil.cc
    src/signaltk/util/exception.cc
    src/signaltk/util/UnixTime.cc
    src/signaltk/util/inspect.cc
    src/signaltk/util/stringutil.cc
    src/signaltk/util/duration.cc
    src/signaltk/util/CivilTime.cc
    src/signaltk/util/buffer.cc
    src/signaltk/util/fileutil.cc
    src/signaltk/util/file.cc
    src/signaltk/util/outputstream.cc
    src/signaltk/util/inputstream.cc
    src/signaltk/util/flagparser.cc
    src/signaltk/util/ISO8601.cc
    src/signaltk/util/UTF8.cc
    src/signaltk/util/wallclock.cc
    src/signaltk_cli.cc
    src/signaltk_cmd.cc
    src/signaltk.cc)

target_link_libraries(signaltk
    ${CAIRO_LIBRARIES})

add_custom_target(check
    ALL
    DEPENDS signaltk
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/test/test_runner.sh")

