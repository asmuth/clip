project(FNORD_FTS)

find_package(Threads)
add_definitions(-DBOOST_THREAD_DONT_USE_CHRONO)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS date_time filesystem iostreams regex system thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB_RECURSE FNORD_FTS_SOURCES
  search/*.cc
  analysis/*.cc
  document/*.cc
  index/*.cc
  queryparser/*.cc
  store/*.cc
  util/*.c*)

set(FNORD_FTS_LDFLAGS
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    ${Boost_SYSTEM_LIBRARIES}
    ${Boost_FILESYSTEM_LIBRARIES}
    ${Boost_IOSTREAMS_LIBRARIES}
    ${Boost_REGEX_LIBRARIES}
    ${Boost_THREAD_LIBRARIES} PARENT_SCOPE)

add_library(fnord-fts OBJECT ${FNORD_FTS_SOURCES})
