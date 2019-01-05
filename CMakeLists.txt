cmake_minimum_required(VERSION 2.8.8)
project(plotfx)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/extra/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads)
find_package(Cairo)
find_package(Freetype)
find_package(HarfBuzz)
find_package(Fontconfig)
find_package(PNG)
include_directories(${CAIRO_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${HARFBUZZ_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS})

add_definitions(-DFNORDMETRIC_VERSION="unstable")

add_library(plotfxlib STATIC
    source/plot.cc
    source/plot_area.cc
    source/plot_axis.cc
    source/plot_bars.cc
    source/plot_gridlines.cc
    source/plot_labels.cc
    source/plot_lines.cc
    source/plot_points.cc
    source/legend.cc
    source/config_helpers.cc
    source/data_model.cc
    source/dimension.cc
    source/domain.cc
    source/document.cc
    source/format.cc
    source/plist/plist.cc
    source/plist/plist_parser.cc
    source/graphics/path.cc
    source/graphics/brush.cc
    source/graphics/color.cc
    source/graphics/geometry.cc
    source/graphics/image.cc
    source/graphics/layer.cc
    source/graphics/layer_pixmap.cc
    source/graphics/layer_svg.cc
    source/graphics/layout.cc
    source/graphics/measure.cc
    source/graphics/text.cc
    source/graphics/text_layout.cc
    source/graphics/text_shaper.cc
    source/graphics/rasterize.cc
    source/graphics/png.cc
    source/graphics/font_lookup.cc
    source/element_factory.cc
    source/utils/random.cc
    source/utils/csv.cc
    source/utils/bufferutil.cc
    source/utils/exception.cc
    source/utils/UnixTime.cc
    source/utils/inspect.cc
    source/utils/stringutil.cc
    source/utils/duration.cc
    source/utils/CivilTime.cc
    source/utils/buffer.cc
    source/utils/fileutil.cc
    source/utils/file.cc
    source/utils/flagparser.cc
    source/utils/ISO8601.cc
    source/utils/UTF8.cc
    source/utils/wallclock.cc)

set(PLOTFX_LDFLAGS plotfxlib ${CAIRO_LIBRARIES} ${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${HARFBUZZ_ICU_LIBRARIES} ${PNG_LIBRARIES} ${FONTCONFIG_LIBRARIES})

add_executable(plotfx source/plotfx_cli.cc)
target_link_libraries(plotfx ${PLOTFX_LDFLAGS})
install(TARGETS plotfx DESTINATION bin)

file(GLOB unit_test_files "tests/unit/**/test_*.cc")
foreach(unit_test_path ${unit_test_files})
  get_filename_component(unit_test_name ${unit_test_path} NAME_WE)
  get_filename_component(unit_test_srcdir ${unit_test_path} DIRECTORY)

  add_executable(${unit_test_name} ${unit_test_path})
  target_link_libraries(${unit_test_name} ${PLOTFX_LDFLAGS})

  add_test(
      NAME ${unit_test_name}
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${unit_test_name})
endforeach()

file(GLOB spec_test_files "tests/spec/**/test_*.ptx")
foreach(spec_test_path ${spec_test_files})
  get_filename_component(spec_test_name ${spec_test_path} NAME_WE)
  get_filename_component(spec_test_srcdir ${spec_test_path} DIRECTORY)
  add_test(
      NAME ${spec_test_name}_svg
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/spec/test_runner.sh ${CMAKE_CURRENT_BINARY_DIR}/plotfx ${spec_test_path} ${CMAKE_CURRENT_BINARY_DIR}/${spec_test_name}.svg ${spec_test_srcdir}/${spec_test_name}.svg)
endforeach()

file(GLOB example_test_files "examples/**/*.ptx")
foreach(example_test_path ${example_test_files})
  get_filename_component(example_test_name ${example_test_path} NAME_WE)
  get_filename_component(example_test_srcdir ${example_test_path} DIRECTORY)
  add_test(
      NAME test_example_${example_test_name}_svg
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/spec/test_runner.sh ${CMAKE_CURRENT_BINARY_DIR}/plotfx ${example_test_path} ${CMAKE_CURRENT_BINARY_DIR}/${example_test_name}.svg ${example_test_srcdir}/${example_test_name}.svg)
endforeach()

