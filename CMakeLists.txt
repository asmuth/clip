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

option(FVIZ_TESTING "Builds with testing flags [default: OFF]" OFF)
option(FVIZ_TESTING "Builds with coverage flags [default: OFF]" OFF)

if(FVIZ_TESTING)
  add_compile_options(
      -Wall
      -Wextra
      -Werror
      -Wno-error=sign-compare
      -Wno-error=reorder
      -Wno-error=unused-parameter
      -Wno-error=unused-variable
      -Wno-error=unused-but-set-variable
      -Wno-error=catch-value
      -Wno-error=switch
      -g
      --coverage
      -O0
      -fsanitize=address
      -DADDRESS_SANITIZER
      -fsanitize=undefined)

  set(CMAKE_EXE_LINKER_FLAGS "--coverage ${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()


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
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/test
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/test-examples)

file(GLOB unit_test_files "tests/unit/test_*.cc")
foreach(unit_test_path ${unit_test_files})
  get_filename_component(unit_test_name ${unit_test_path} NAME_WE)
  get_filename_component(unit_test_srcdir ${unit_test_path} DIRECTORY)

  add_executable(${unit_test_name} ${unit_test_path})
  target_link_libraries(${unit_test_name} ${fviz_LDFLAGS})

  add_test(
      NAME test_unit_${unit_test_name}
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${unit_test_name}
      DEPENDS fviz-cli test_dir)
endforeach()

file(GLOB spec_test_files "tests/spec/*.fvz")
foreach(spec_test_path ${spec_test_files})
  get_filename_component(spec_test_name ${spec_test_path} NAME_WE)
  get_filename_component(spec_test_srcdir ${spec_test_path} DIRECTORY)
  add_test(
      NAME test_spec_${spec_test_name}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_runner.sh ${CMAKE_CURRENT_BINARY_DIR}/fviz ${spec_test_name} ${spec_test_srcdir} ${CMAKE_CURRENT_BINARY_DIR}/test
      DEPENDS fviz-cli test_dir)
endforeach()


# Examples
# -----------------------------------------------------------------------------
add_custom_target(examples)
file(GLOB example_files "examples/**/*.fvz")
foreach(example_path ${example_files})
  get_filename_component(example_name ${example_path} NAME_WE)
  get_filename_component(example_srcdir ${example_path} DIRECTORY)
  add_test(
      NAME test_example_${example_name}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_runner.sh ${CMAKE_CURRENT_BINARY_DIR}/fviz ${example_name} ${example_srcdir} ${CMAKE_CURRENT_BINARY_DIR}/test-examples
      DEPENDS fviz-cli test_dir)
  add_custom_target(
      example_${example_name}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/fviz --in ${example_srcdir}/${example_name}.fvz --out ${example_srcdir}/${example_name}.svg)
  add_dependencies(examples example_${example_name})
  add_dependencies(example_${example_name} fviz-cli)
endforeach()


# Documentation
# -----------------------------------------------------------------------------
add_custom_target(docs
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/wwwdocs
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/manual/web/build.sh ${CMAKE_CURRENT_BINARY_DIR}/wwwdocs
    DEPENDS examples)

