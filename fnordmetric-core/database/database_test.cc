/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "database.h"
#include "pagemanager.h"

namespace fnordmetric {
namespace database {

class DatabaseTest {
public:
  DatabaseTest() {}

  void run() {
    //testStreamIdAssignment();
    //testStreamRefCreation();
    //testPageManager();
    //testMmapPageManager();
    testOpenFile();
  }

  void testStreamIdAssignment() {
    auto backend = fnordmetric::database::Database::openFile(
        "/tmp/__fnordmetric_testStreamIdAssignment");

    std::string key1 = "83d2f71c457206bf-Ia9f37ed7-F76b77d1a";
    std::string key2 = "83d2f71c457216bf-Ia9f37ed7-F76b77d1a";

    assert(backend->getStreamId(key1) == 1);
    assert(backend->getStreamId(key2) == 2);
    assert(backend->getStreamId(key1) == 1);
    assert(backend->getStreamId(key2) == 2);
  }

  void testStreamRefCreation() {
    auto backend = fnordmetric::database::Database::openFile(
        "/tmp/__fnordmetric_testStreamRefCreation");

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

  void testPageManager() {
    class ConcreteTestPageManager : public PageManager {
    public:
      ConcreteTestPageManager() : PageManager(4096) {}
      std::unique_ptr<PageRef> getPage(const PageManager::Page& page) override {
        return std::unique_ptr<PageRef>(nullptr);
      }
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

  void testOpenFile() {
    uint32_t stream_id;
    std::vector<uint8_t> data = {
        0x01, 0x02, 0x03, 0x04,
        0x05, 0x06, 0x07, 0x08
    };

    {
      auto database = fnordmetric::database::Database::openFile(
          "/tmp/__fnordmetric_testOpenFile");
      assert(database.get() != nullptr);
      auto streamdesc = database->openStream("mystream");
      stream_id = streamdesc->stream_id_;
      streamdesc->appendRow(data);
      streamdesc->appendRow(data);
    }

    auto database = fnordmetric::database::Database::openFile(
        "/tmp/__fnordmetric_testOpenFile");
    assert(database.get() != nullptr);
    assert(database->max_stream_id_ == stream_id);
    auto streamdesc = database->openStream("mystream");
    assert(stream_id == streamdesc->stream_id_);
  }

};

}
}

int main() {
  fnordmetric::database::DatabaseTest test;
  test.run();
  printf("all tests passed! :)\n");
}
