cmake_minimum_required(VERSION 3.8)
project(clip VERSION "0.8.0")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/extra/cmake")


# Options
# -----------------------------------------------------------------------------
option(CLIP_TEXT_ENABLE_BIDI "Enable bi-directional text support" ON)
set(CLIP_TEXT_BACKEND "freetype" CACHE STRING "Set the text backend")
option(CLIP_TESTING "Builds with testing flags [default: OFF]" OFF)


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

#if(CLIP_TEXT_BACKEND EQUAL "freetype")
  pkg_check_modules(FRIBIDI QUIET fribidi)
  find_path(FRIBIDI_INCLUDE_DIR NAMES fribidi/fribidi.h)
  find_library(FRIBIDI_LIBRARY NAMES fribidi)

  if(FRIBIDI_FOUND)
    set(FRIBIDI_LIBRARIES "${FRIBIDI_LIBRARY}")
  else()
    message(WARNING "libfribidi not found; disabling BiDI support")
    set(CLIP_TEXT_ENABLE_BIDI OFF)
  endif()
#endif()


# Compile Flags
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
include_directories(${CAIRO_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS} ${HARFBUZZ_INCLUDE_DIRS} ${PNG_INCLUDE_DIRS}})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/utils)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(CLIP_TESTING)
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
configure_file(src/config.h.in config.h)
message(STATUS "Config Options: text_backend=${CLIP_TEXT_BACKEND}")
message(STATUS "Config Options: text_enable_bidi=${CLIP_TEXT_ENABLE_BIDI}")


# Build: clip Library
# -----------------------------------------------------------------------------
file(GLOB source_files "src/*.cc" "src/**/*.cc" "src/elements/**/*.cc" )
list(REMOVE_ITEM source_files "${CMAKE_SOURCE_DIR}/src/cli.cc")
list(REMOVE_ITEM source_files "${CMAKE_SOURCE_DIR}/src/environment.cc")
list(REMOVE_ITEM source_files "${CMAKE_SOURCE_DIR}/src/layout/box.cc")
list(REMOVE_ITEM source_files "${CMAKE_SOURCE_DIR}/src/plot.cc")
add_library(clip OBJECT ${source_files})
set_property(TARGET clip PROPERTY POSITION_INDEPENDENT_CODE 1)
set(CLIP_LIB_LDFLAGS ${CAIRO_LIBRARIES} ${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${HARFBUZZ_ICU_LIBRARIES} ${PNG_LIBRARIES} ${FONTCONFIG_LIBRARIES} ${FRIBIDI_LIBRARIES} ${fmt_LIBRARY})
add_library(clip-lib-a STATIC $<TARGET_OBJECTS:clip>)
set_target_properties(clip-lib-a PROPERTIES OUTPUT_NAME clip)
add_library(clip-lib-so SHARED $<TARGET_OBJECTS:clip>)
target_link_libraries(clip-lib-so ${CLIP_LIB_LDFLAGS})
set_target_properties(clip-lib-so PROPERTIES OUTPUT_NAME clip)
set_target_properties(clip-lib-so PROPERTIES PUBLIC_HEADER "src/api.h")
set_target_properties(clip-lib-so PROPERTIES CXX_VISIBILITY_PRESET hidden)

# Build: CLI
# -----------------------------------------------------------------------------
add_executable(clip-cli "src/cli.cc" $<TARGET_OBJECTS:clip>)
target_link_libraries(clip-cli ${CLIP_LIB_LDFLAGS})
set_target_properties(clip-cli PROPERTIES OUTPUT_NAME clip)


# Installation
# -----------------------------------------------------------------------------
install(TARGETS clip-lib-a ARCHIVE DESTINATION lib PUBLIC_HEADER DESTINATION include/clip)
install(TARGETS clip-lib-so LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION include/clip)
install(TARGETS clip-cli DESTINATION bin)


# Testing
# -----------------------------------------------------------------------------
add_custom_target(test
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test/test_runner.sh
    DEPENDS clip-cli)

add_custom_target(test-extract-data ${CMAKE_CURRENT_SOURCE_DIR}/test/testdata/extract_testdata.sh)
add_dependencies(test test-extract-data)


# Documentation
# -----------------------------------------------------------------------------
add_custom_target(doc
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/wwwdocs
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/doc/web/build.sh ${clip_VERSION} ${CMAKE_CURRENT_BINARY_DIR}/wwwdocs)

file(GLOB doc_figure_files "doc/**/*.clp")
foreach(doc_figure_path ${doc_figure_files})
  get_filename_component(doc_figure_name ${doc_figure_path} NAME_WE)
  get_filename_component(doc_figure_srcdir ${doc_figure_path} DIRECTORY)
  add_custom_target(
      doc_figure_${doc_figure_name}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${CMAKE_CURRENT_BINARY_DIR}/clip ${doc_figure_srcdir}/${doc_figure_name}.clp -e ${doc_figure_srcdir}/${doc_figure_name}.svg)
  add_dependencies(doc_figure_${doc_figure_name} clip-cli)
  add_dependencies(doc doc_figure_${doc_figure_name})
endforeach()


