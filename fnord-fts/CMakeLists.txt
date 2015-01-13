project(lucene++)

cmake_minimum_required(VERSION 2.8.6)

set(lucene++_VERSION_MAJOR 3)
set(lucene++_VERSION_MINOR 0)
set(lucene++_VERSION_PATCH 6)

set(lucene++_SOVERSION "0")

set(lucene++_VERSION
  "${lucene++_VERSION_MAJOR}.${lucene++_VERSION_MINOR}.${lucene++_VERSION_PATCH}"
)

option(LUCENE_USE_STATIC_BOOST_LIBS
  "Use static boost libraries"
  OFF
)

option(ENABLE_CYCLIC_CHECK
  "Enable cyclic checking"
  OFF
)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS
  date_time
  filesystem
  iostreams
  regex
  system
  thread
  REQUIRED
)

add_definitions( -DBOOST_THREAD_DONT_USE_CHRONO )
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ${LUCENE_USE_STATIC_BOOST_LIBS})

set(lucene_boost_libs
  ${Boost_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARIES}
  ${Boost_IOSTREAMS_LIBRARIES}
  ${Boost_REGEX_LIBRARIES}
  ${Boost_SYSTEM_LIBRARIES}
  ${Boost_THREAD_LIBRARIES}
)

if(ENABLE_CYCLIC_CHECK)
  set(DEFINE_USE_CYCLIC_CHECK "define")
else()
  set(DEFINE_USE_CYCLIC_CHECK "undef")
endif()

#add_subdirectory(core)
#add_subdirectory(test)

include_directories(
  "${lucene++_SOURCE_DIR}/include"
  ${Boost_INCLUDE_DIRS}
)

file(GLOB_RECURSE lucene_sources
  search/*.cpp
  analysis/*.cpp
  document/*.cpp
  index/*.cpp
  queryparser/*.cpp
  store/*.cpp
  util/*.c*)

file(GLOB_RECURSE lucene_internal_headers
  "${lucene++-lib_SOURCE_DIR}/include/*.h"
)

file(GLOB_RECURSE lucene_headers
  "${lucene++_SOURCE_DIR}/include/*.h"
)

add_definitions(-DLPP_BUILDING_LIB)

add_library(fnord-fts OBJECT  ${lucene_sources})

#if(WIN32 OR WIN64)
#  set(CMAKE_DEBUG_POSTFIX "d")
#endif()

#if(NOT WIN32 AND NOT CMAKE_SYSTEM MATCHES "SunOS-5*.")
#  add_definitions(-fPIC)
#endif()
#
#if(CYGWIN)
#  add_definitions(-D__LARGE64_FILES)
#endif()

#if(APPLE)
#  set(CMAKE_MACOSX_RPATH ON)
#  set(CMAKE_SKIP_BUILD_RPATH FALSE)
#  set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
#  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#  list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
#  if("${isSystemDir}" STREQUAL "-1")
#    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
#  endif()
#endif()

#configure_file(
#  "${CMAKE_CURRENT_SOURCE_DIR}/include/Config.h.cmake"
#  "${CMAKE_CURRENT_BINARY_DIR}/include/Config.h" @ONLY
#)
