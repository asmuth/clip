project(FNORD_FEEDS)

include_directories(../)
include_directories(../3rdparty)

add_library(fnord-feeds OBJECT
    LocalFeed.cc
    RemoteFeed.cc
    RemoteFeedFactory.cc
    RemoteFeedReader.cc
    RemoteFeedWriter.cc
    FeedService.cc)

add_executable(fn-feedtail
    $<TARGET_OBJECTS:fnord-base>
    $<TARGET_OBJECTS:fnord-http>
    $<TARGET_OBJECTS:fnord-rpc>
    $<TARGET_OBJECTS:fnord-json>
    $<TARGET_OBJECTS:fnord-sstable>
    $<TARGET_OBJECTS:fnord-feeds>
    fn-feedtail.cc)

target_link_libraries(fn-feedtail ${CMAKE_THREAD_LIBS_INIT})

#add_executable(fnord-logstream-service-example
#    $<TARGET_OBJECTS:fnord-base>
#    $<TARGET_OBJECTS:fnord-feeds>
#    $<TARGET_OBJECTS:fnord-http>
#    $<TARGET_OBJECTS:fnord-json>
#    $<TARGET_OBJECTS:fnord-sstable>
#    feeds_example.cc)
#
#add_executable(tests/test-logstream-service
#    $<TARGET_OBJECTS:fnord-base>
#    $<TARGET_OBJECTS:fnord-feeds>
#    $<TARGET_OBJECTS:fnord-http>
#    $<TARGET_OBJECTS:fnord-json>
#    $<TARGET_OBJECTS:fnord-sstable>
#    feeds_test.cc)
#
