/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "collection.h"
#include "pagemanager.h"
#include "transaction.h"
//#include "cursor.h"
//#include "clock.h"

namespace fnordmetric {

// TODO test freelist serialization / after file reloads
// TODO test in memory db

class DatabaseTest {
public:
  DatabaseTest() {}

  void run() {
    //testStreamIdAssignment();
    //testStreamRefCreation();
    testPageManager();
    testMmapPageManager();
    testAOCollection();
  }

/*
  void testStreamIdAssignment() {
    auto backend = fnordmetric::database::Database::openFile(
        "/tmp/__fnordmetric_testStreamIdAssignment",
        database::MODE_CONSERVATIVE | database::FILE_TRUNCATE);

    std::string key1 = "83d2f71c457206bf-Ia9f37ed7-F76b77d1a";
    std::string key2 = "83d2f71c457216bf-Ia9f37ed7-F76b77d1a";

    assert(backend->getStreamId(key1) == 1);
    assert(backend->getStreamId(key2) == 2);
    assert(backend->getStreamId(key1) == 1);
    assert(backend->getStreamId(key2) == 2);
  }

  void testStreamRefCreation() {
    auto backend = fnordmetric::database::Database::openFile(
        "/tmp/__fnordmetric_testStreamRefCreation",
        database::MODE_CONSERVATIVE | database::FILE_TRUNCATE |
        database::FILE_AUTODELETE);

    std::string key1 = "83d2f71c457206bf-Ia9f37ed7-F76b77d1a";
    std::string key2 = "83d2f71c457216bf-Ia9f37ed7-F76b77d1a";
    auto ref1 = backend->openStream(key1);
    assert(ref1.get() != nullptr);
    auto ref2 = backend->openStream(key1);
    assert(ref1.get() == ref2.get());
    auto ref3 = backend->openStream(key2);
    assert(ref1.get() != ref3.get());
    assert(ref2.get() != ref3.get());
    auto ref4 = backend->openStream(key2);
    assert(ref1.get() != ref4.get());
    assert(ref2.get() != ref4.get());
    assert(ref3.get() == ref4.get());
  }
*/

  void testPageManager() {
    class ConcreteTestPageManager : public PageManager {
    public:
      ConcreteTestPageManager() : PageManager(4096) {}
      std::unique_ptr<PageRef> getPage(const PageManager::Page& page) override {
        return std::unique_ptr<PageRef>(nullptr);
      }
      void fsync() const {}
    };
    ConcreteTestPageManager page_manager;

    auto page1 = page_manager.allocPage(3000);
    assert(page_manager.end_pos_ == 4096);
    assert(page1.offset == 0);
    assert(page1.size == 4096);
    page_manager.freePage(page1);

    auto page2 = page_manager.allocPage(8000);
    assert(page_manager.end_pos_ == 12288);
    assert(page2.offset == 4096);
    assert(page2.size == 8192);

    auto page3 = page_manager.allocPage(3000);
    assert(page3.offset == 0);
    assert(page3.size == 4096);

    page_manager.freePage(page2);
    auto page4 = page_manager.allocPage(4000);
    assert(page_manager.end_pos_ == 12288);
    assert(page4.offset == 4096);
    assert(page4.size == 8192);
  }

  void testMmapPageManager() {
    int fd = open("/tmp/__fnordmetric_testMmapPageManager",
        O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    assert(fd > 0);
    unlink("/tmp/__fnordmetric_testMmapPageManager");
    auto page_manager = new MmapPageManager(fd, 0, 4096);

    auto mfile1 = page_manager->getMmapedFile(3000);
    auto mfile2 = page_manager->getMmapedFile(304200);
    assert(mfile1->size == 1048576);
    assert(mfile1 == mfile2);
    mfile2->incrRefs();
    auto mfile3 = page_manager->getMmapedFile(1048577);
    assert(mfile3->size == 1048576 * 2);
    assert(mfile3 != mfile2);
    mfile2->decrRefs();

    delete page_manager;
    close(fd);
  }

  void testAOCollection() {
    auto collection = Collection::createPersistentCollection(
        "/tmp/__fnordmetric_testAOCollection",
        Collection::FILE_TRUNCATE);

    //auto tx = collection->startTransaction();
    //auto docref = tx->createDocument();
    //auto docref1 = tx->createDocument();
    //auto docref2 = tx->createDocument();
    //auto docref3 = tx->createDocument();
    //tx->commit();
    //printf("committed...\n");
    //collection->sync();
    //printf("synced...\n");
/*
    //printf("TEST: File backed database insert, reopen, read\n");
    uint32_t stream_id;
    std::vector<fnordmetric::database::StreamPosition> insert_times;
    std::vector<Field> fields = {
        fnordmetric::IntegerField("sequence_num"),
        fnordmetric::IntegerField("test1"),
        fnordmetric::StringField("test2")};
    Schema schema(fields);

    int rows_written = 0;
    size_t base_time = WallClock::getUnixMillis();

    //for (int j = 0; j < 50; ++j) {
    for (int j = 0; j < 5; ++j) {
      int flags = database::MODE_CONSERVATIVE;
      if (j == 0) { flags |= database::FILE_TRUNCATE; }
      if (j == 49) { flags |= database::FILE_AUTODELETE; }
      auto database = fnordmetric::database::Database::openFile(
          "/tmp/__fnordmetric_testOpenFile",
          flags);
      assert(database.get() != nullptr);
      auto stream = database->openStream("mystream");
      if (j == 0) {
        stream_id = stream->stream_id_;
      } else {
        assert(stream_id == stream->stream_id_);
      }
      assert(database->max_stream_id_ == stream_id);
      RecordWriter record_writer(schema);
      for (int i = (j + 1) * 1000; i > 0; i--) {
        auto insert_time = base_time + rows_written;
        record_writer.setIntegerField(0, ++rows_written);
        record_writer.setIntegerField(1, 1337);
        record_writer.setStringField(2, "fnordbar", 8);
        auto new_row_pos = stream->appendRow(record_writer, insert_time);
        if (insert_times.size() > 0) {
          assert(
              new_row_pos.logical_offset > insert_times.back().logical_offset);
        }
        insert_times.push_back(new_row_pos);
        record_writer.reset();
      }
      auto cursor = stream->getCursor();
      assert(cursor->seekToLast() == insert_times.back());
      auto test_index = insert_times.size() / 2;
      auto test_pos = cursor->seekToLogicalOffset(
          insert_times[test_index].logical_offset);
      assert(test_pos == insert_times[test_index]);
      test_pos = cursor->seekToLogicalOffset(
          insert_times[test_index].logical_offset + 1);
      assert(test_pos == insert_times[test_index + 1]);
      assert(cursor->seekToFirst() == insert_times[0]);
      test_pos = cursor->seekToTime(insert_times[test_index].unix_millis);
      assert(test_pos == insert_times[test_index]);
      assert(cursor->seekToFirst() == insert_times[0]);
      RecordReader record_reader(schema);
      for (int i = 0; i < insert_times.size() - 1; ++i) {
        auto row = cursor->getCurrentRow();
        assert(cursor->getCurrentPosition() == insert_times[i]);
        assert(row->time == insert_times[i].unix_millis);
        assert(cursor->next());
        assert(record_reader.getIntegerField(row->data, 0) == i+1);
        assert(record_reader.getIntegerField(row->data, 1) == 1337);
        char* str;
        size_t str_len;
        record_reader.getStringField(row->data, 2, &str, &str_len);
        assert(str_len == 8);
        assert(strncmp(str, "fnordbar", str_len) == 0);
      }
      assert(cursor->next() == false);
      query::Query query("SELECT count(*) FROM mystream");
      database->executeQuery(&query);
    }
*/
  }

};

}

int main() {
  fnordmetric::DatabaseTest test;
  test.run();
  printf("all tests passed! :)\n");
}
