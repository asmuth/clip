cmake_minimum_required(VERSION 2.8.8)
project(signaltk)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/extra/cmake")
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads)
find_package(Cairo)
find_package(Freetype)
find_package(HarfBuzz)
find_package(PNG)
include_directories(${CAIRO_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${HARFBUZZ_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS})

add_definitions(-DFNORDMETRIC_VERSION="unstable")

add_library(signaltk STATIC
    src/signaltk/core/path.cc
    src/signaltk/core/brush.cc
    src/signaltk/core/colour.cc
    src/signaltk/core/image.cc
    src/signaltk/core/layer.cc
    src/signaltk/core/text.cc
    src/signaltk/core/rasterize.cc
    src/signaltk/codec/png.cc
    src/signaltk/plot/gridlines.cc
    src/signaltk/plot/axes.cc
    src/signaltk/plot/domain.cc
    src/signaltk/plot/domainprovider.cc
    src/signaltk/plot/legenddefinition.cc
    src/signaltk/plot/series.cc
    src/signaltk/plot/timedomain.cc
    src/signaltk/text/text_layout.cc
    src/signaltk/text/text_shaper.cc
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
    src/signaltk_cmd.cc)

set(SIGNALTK_LDFLAGS signaltk ${CAIRO_LIBRARIES} ${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${HARFBUZZ_ICU_LIBRARIES} ${PNG_LIBRARIES} agg)

file(GLOB test_files "test/**/test_*.cc")
foreach(test_path ${test_files})
  get_filename_component(test_name ${test_path} NAME_WE)
  get_filename_component(test_srcdir ${test_path} DIRECTORY)

  add_executable(${test_name} ${test_path})
  target_link_libraries(${test_name} ${SIGNALTK_LDFLAGS})

  add_test(
      NAME ${test_name}
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/test_runner.sh ${CMAKE_CURRENT_BINARY_DIR}/${test_name} ${test_srcdir}/${test_name}.png)
endforeach()
