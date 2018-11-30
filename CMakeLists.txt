cmake_minimum_required(VERSION 2.8.8)
project(plotfx)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/extra/cmake")
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads)
find_package(Cairo)
find_package(Freetype)
find_package(HarfBuzz)
find_package(PNG)
include_directories(${CAIRO_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${HARFBUZZ_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS})

add_definitions(-DFNORDMETRIC_VERSION="unstable")

add_library(plotfxlib STATIC
    charts/linechart.cc
    charts/gridlines.cc
    charts/plot_axis.cc
    charts/plot_domain.cc
    charts/legenddefinition.cc
    charts/series.cc
    common/config_helpers.cc
    common/plist/plist.cc
    common/plist/plist_parser.cc
    common/graphics/path.cc
    common/graphics/brush.cc
    common/graphics/colour.cc
    common/graphics/image.cc
    common/graphics/layer.cc
    common/graphics/layout.cc
    common/graphics/measure.cc
    common/graphics/text.cc
    common/graphics/text_layout.cc
    common/graphics/text_shaper.cc
    common/graphics/rasterize.cc
    common/graphics/png.cc
    common/element_factory.cc
    common/element_tree.cc
    common/utils/random.cc
    common/utils/bufferutil.cc
    common/utils/exception.cc
    common/utils/UnixTime.cc
    common/utils/inspect.cc
    common/utils/stringutil.cc
    common/utils/duration.cc
    common/utils/CivilTime.cc
    common/utils/buffer.cc
    common/utils/fileutil.cc
    common/utils/file.cc
    common/utils/outputstream.cc
    common/utils/inputstream.cc
    common/utils/flagparser.cc
    common/utils/ISO8601.cc
    common/utils/UTF8.cc
    common/utils/wallclock.cc
    plotfx_cmd.cc)

set(PLOTFX_LDFLAGS plotfxlib ${CAIRO_LIBRARIES} ${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${HARFBUZZ_ICU_LIBRARIES} ${PNG_LIBRARIES})

add_executable(plotfx common/platform/plotfx_cli.cc)
target_link_libraries(plotfx ${PLOTFX_LDFLAGS})

file(GLOB unit_test_files "tests/**/test_*.cc")
foreach(unit_test_path ${unit_test_files})
  get_filename_component(unit_test_name ${unit_test_path} NAME_WE)
  get_filename_component(unit_test_srcdir ${unit_test_path} DIRECTORY)

  add_executable(${unit_test_name} ${unit_test_path})
  target_link_libraries(${unit_test_name} ${PLOTFX_LDFLAGS})

  add_test(
      NAME ${unit_test_name}
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${unit_test_name})
endforeach()

file(GLOB spec_test_files "tests/**/test_*.spec")
foreach(spec_test_path ${spec_test_files})
  get_filename_component(spec_test_name ${spec_test_path} NAME_WE)
  get_filename_component(spec_test_srcdir ${spec_test_path} DIRECTORY)
  add_test(
      NAME ${spec_test_name}
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_runner.sh ${CMAKE_CURRENT_BINARY_DIR}/plotfx ${spec_test_path} ${CMAKE_CURRENT_BINARY_DIR}/${spec_test_name}.png ${spec_test_srcdir}/${spec_test_name}.png)
endforeach()
