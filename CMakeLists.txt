cmake_minimum_required(VERSION 3.8)
project(fviz VERSION "0.5.0")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/platform/cmake")


# Options
# -----------------------------------------------------------------------------
option(FVIZ_TEXT_ENABLE_BIDI "Enable bi-directional text support" ON)
set(FVIZ_TEXT_BACKEND "freetype" CACHE STRING "Set the text backend")
option(FVIZ_TESTING "Builds with testing flags [default: OFF]" OFF)


# Dependencies
# -----------------------------------------------------------------------------
find_package(Threads)
find_package(Cairo REQUIRED)
find_package(Freetype REQUIRED)
find_package(HarfBuzz REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(PNG REQUIRED)

find_package(fmt REQUIRED)
find_library(fmt_LIBRARY NAMES fmt)

#if(FVIZ_TEXT_BACKEND EQUAL "freetype")
  pkg_check_modules(FRIBIDI QUIET fribidi)
  find_path(FRIBIDI_INCLUDE_DIR NAMES fribidi/fribidi.h)
  find_library(FRIBIDI_LIBRARY NAMES fribidi)

  if(FRIBIDI_FOUND)
    set(FRIBIDI_LIBRARIES "${FRIBIDI_LIBRARY}")
  else()
    message(WARNING "libfribidi not found; disabling BiDI support")
    set(FVIZ_TEXT_ENABLE_BIDI OFF)
  endif()
#endif()


# Compile Flags
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
include_directories(${CAIRO_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${HARFBUZZ_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS}})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

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


# Build: Config
# -----------------------------------------------------------------------------
configure_file(core/config.h.in config.h)
message(STATUS "Config Options: text_backend=${FVIZ_TEXT_BACKEND}")
message(STATUS "Config Options: text_enable_bidi=${FVIZ_TEXT_ENABLE_BIDI}")


# Build: fviz Library
# -----------------------------------------------------------------------------
file(GLOB source_files "core/*.cc" "core/**/*.cc" "elements/*.cc" "elements/**/*.cc")
list(REMOVE_ITEM source_files "${CMAKE_SOURCE_DIR}/core/cli.cc")
add_library(fviz OBJECT ${source_files})
set_property(TARGET fviz PROPERTY POSITION_INDEPENDENT_CODE 1)
set(FVIZ_LIB_LDFLAGS ${CAIRO_LIBRARIES} ${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${HARFBUZZ_ICU_LIBRARIES} ${PNG_LIBRARIES} ${FONTCONFIG_LIBRARIES} ${FRIBIDI_LIBRARIES} ${fmt_LIBRARY})
add_library(fviz-lib-a STATIC $<TARGET_OBJECTS:fviz>)
set_target_properties(fviz-lib-a PROPERTIES OUTPUT_NAME fviz)
add_library(fviz-lib-so SHARED $<TARGET_OBJECTS:fviz>)
target_link_libraries(fviz-lib-so ${FVIZ_LIB_LDFLAGS})
set_target_properties(fviz-lib-so PROPERTIES OUTPUT_NAME fviz)
set_target_properties(fviz-lib-so PROPERTIES PUBLIC_HEADER "core/api.h")
set_target_properties(fviz-lib-so PROPERTIES CXX_VISIBILITY_PRESET hidden)

# Build: CLI
# -----------------------------------------------------------------------------
add_executable(fviz-cli "core/cli.cc" $<TARGET_OBJECTS:fviz>)
target_link_libraries(fviz-cli ${FVIZ_LIB_LDFLAGS})
set_target_properties(fviz-cli PROPERTIES OUTPUT_NAME fviz)


# Installation
# -----------------------------------------------------------------------------
install(TARGETS fviz-lib-a ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include/fviz)
install(TARGETS fviz-lib-so LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/fviz)
install(TARGETS fviz-cli DESTINATION bin)


# Testing
# -----------------------------------------------------------------------------
add_custom_target(test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_runner.sh
    DEPENDS fviz-cli)

add_custom_target(test-examples ${CMAKE_CURRENT_SOURCE_DIR}/examples/update_tests.sh)
add_dependencies(test test-examples)


# Examples
# -----------------------------------------------------------------------------
add_custom_target(examples)
file(GLOB example_files "examples/**/*.fvz")
foreach(example_path ${example_files})
  get_filename_component(example_name ${example_path} NAME_WE)
  get_filename_component(example_srcdir ${example_path} DIRECTORY)
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
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/manual/web/build.sh ${CMAKE_CURRENT_BINARY_DIR}/wwwdocs)

