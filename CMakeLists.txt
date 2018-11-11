cmake_minimum_required(VERSION 2.8.8)
project(signaltk)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/extra/cmake")
set(CMAKE_CXX_STANDARD 17)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/toolkit)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads)
find_package(Cairo)
find_package(Freetype)
find_package(HarfBuzz)
find_package(PNG)
include_directories(${CAIRO_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${HARFBUZZ_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS})

add_definitions(-DFNORDMETRIC_VERSION="unstable")

add_library(signaltk STATIC
    graphics/path.cc
    graphics/brush.cc
    graphics/colour.cc
    graphics/image.cc
    graphics/layer.cc
    graphics/text.cc
    graphics/text_layout.cc
    graphics/text_shaper.cc
    graphics/rasterize.cc
    graphics/png.cc
    elements/element_factory.cc
    elements/element_spec_parser.cc
    elements/element_tree.cc
    elements/plot/gridlines.cc
    elements/plot/plot_axis.cc
    elements/plot/plot_domain.cc
    elements/plot/plot_element.cc
    elements/plot/legenddefinition.cc
    elements/plot/series.cc
    utils/random.cc
    utils/bufferutil.cc
    utils/exception.cc
    utils/UnixTime.cc
    utils/inspect.cc
    utils/stringutil.cc
    utils/duration.cc
    utils/CivilTime.cc
    utils/buffer.cc
    utils/fileutil.cc
    utils/file.cc
    utils/outputstream.cc
    utils/inputstream.cc
    utils/flagparser.cc
    utils/ISO8601.cc
    utils/UTF8.cc
    utils/wallclock.cc
    signaltk_cmd.cc)

set(SIGNALTK_LDFLAGS signaltk ${CAIRO_LIBRARIES} ${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${HARFBUZZ_ICU_LIBRARIES} ${PNG_LIBRARIES})

add_executable(plotfx platform/signaltk_cli.cc)
target_link_libraries(plotfx ${SIGNALTK_LDFLAGS})

file(GLOB unit_test_files "testing/**/test_*.cc")
foreach(unit_test_path ${unit_test_files})
  get_filename_component(unit_test_name ${unit_test_path} NAME_WE)
  get_filename_component(unit_test_srcdir ${unit_test_path} DIRECTORY)

  add_executable(${unit_test_name} ${unit_test_path})
  target_link_libraries(${unit_test_name} ${SIGNALTK_LDFLAGS})

  add_test(
      NAME ${unit_test_name}
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${unit_test_name})
endforeach()

file(GLOB spec_test_files "testing/**/test_*.spec")
foreach(spec_test_path ${spec_test_files})
  get_filename_component(spec_test_name ${spec_test_path} NAME_WE)
  get_filename_component(spec_test_srcdir ${spec_test_path} DIRECTORY)
  add_test(
      NAME ${spec_test_name}
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testing/test_runner.sh ${CMAKE_CURRENT_BINARY_DIR}/plotfx ${spec_test_path} ${CMAKE_CURRENT_BINARY_DIR}/${spec_test_name}.png ${spec_test_srcdir}/${spec_test_name}.png)
endforeach()
