project(fviz)
add_definitions("-Dfviz_VERSION='\"v0.4.0\"'")


# CMake Setup
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.8)
enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/platform/cmake")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


# Dependencies
# -----------------------------------------------------------------------------
find_package(Threads)
find_package(Cairo)
find_package(Freetype)
find_package(HarfBuzz)
find_package(Fontconfig)
find_package(PNG)
include_directories(${CAIRO_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${HARFBUZZ_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS})


# Build: fviz Library
# -----------------------------------------------------------------------------
file(GLOB source_files "core/*.cc" "core/**/*.cc" "elements/*.cc" "elements/**/*.cc")
list(REMOVE_ITEM source_files "core/cli.cc")
add_library(fviz STATIC ${source_files})
set_target_properties(fviz PROPERTIES PUBLIC_HEADER "core/fviz.h")
set(fviz_LDFLAGS fviz ${CAIRO_LIBRARIES} ${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${HARFBUZZ_ICU_LIBRARIES} ${PNG_LIBRARIES} ${FONTCONFIG_LIBRARIES} fmt)


# Build: CLI
# -----------------------------------------------------------------------------
add_executable(fviz-cli core/cli.cc)
target_link_libraries(fviz-cli ${fviz_LDFLAGS})
set_target_properties(fviz-cli PROPERTIES OUTPUT_NAME fviz)


# Installation
# -----------------------------------------------------------------------------
install(TARGETS fviz ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include/fviz)
install(TARGETS fviz-cli DESTINATION bin)


# Testing
# -----------------------------------------------------------------------------
add_custom_target(test_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/test)

file(GLOB unit_test_files "tests/unit/test_*.cc")
foreach(unit_test_path ${unit_test_files})
  get_filename_component(unit_test_name ${unit_test_path} NAME_WE)
  get_filename_component(unit_test_srcdir ${unit_test_path} DIRECTORY)

  add_executable(${unit_test_name} ${unit_test_path})
  target_link_libraries(${unit_test_name} ${fviz_LDFLAGS})

  add_test(
      NAME ${unit_test_name}
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${unit_test_name}
      DEPENDS fviz-cli test_dir)
endforeach()

file(GLOB spec_test_files "tests/spec/**/*.fvz")
foreach(spec_test_path ${spec_test_files})
  get_filename_component(spec_test_name ${spec_test_path} NAME_WE)
  get_filename_component(spec_test_srcdir ${spec_test_path} DIRECTORY)
  add_test(
      NAME ${spec_test_name}_svg
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/spec/test_runner.sh ${CMAKE_CURRENT_BINARY_DIR}/fviz ${spec_test_name} ${spec_test_srcdir} ${CMAKE_CURRENT_BINARY_DIR}/test
      DEPENDS fviz-cli test_dir)
endforeach()


# Examples
# -----------------------------------------------------------------------------
add_custom_target(examples)
add_custom_target(examples_dir ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/examples)

file(GLOB example_files "examples/**/*.fvz")
foreach(example_path ${example_files})
  get_filename_component(example_name ${example_path} NAME_WE)
  get_filename_component(example_srcdir ${example_path} DIRECTORY)
  add_custom_target(
      example_${example_name}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/spec/test_runner.sh ${CMAKE_CURRENT_BINARY_DIR}/fviz ${example_name} ${example_srcdir} ${CMAKE_CURRENT_BINARY_DIR}/examples)
  add_dependencies(examples example_${example_name})
  add_dependencies(example_${example_name} fviz)
  add_dependencies(example_${example_name} examples_dir)
endforeach()


# Documentation
# -----------------------------------------------------------------------------
add_custom_target(docs
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/wwwdocs
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/manual/web/build.sh ${CMAKE_CURRENT_BINARY_DIR}/wwwdocs)

file(GLOB figure_files "manual/figures/*.fvz")
foreach(figure_path ${figure_files})
  get_filename_component(figure_name ${figure_path} NAME_WE)
  get_filename_component(figure_srcdir ${figure_path} DIRECTORY)
  add_test(
      NAME test_figure_${figure_name}_svg
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/spec/test_runner.sh ${CMAKE_CURRENT_BINARY_DIR}/fviz ${figure_name} ${figure_srcdir} ${CMAKE_CURRENT_BINARY_DIR})
  add_test(
      figure_${figure_name}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/spec/test_runner.sh ${CMAKE_CURRENT_BINARY_DIR}/fviz ${figure_name} ${figure_srcdir} ${CMAKE_CURRENT_BINARY_DIR})
  add_dependencies(docs figure_${figure_name})
endforeach()

