/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/io/pagemanager.h>
#include <fnordmetric/util/unittest.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>

using namespace fnordmetric::io;

UNIT_TEST(PageManagerTest);

class ConcreteTestPageManager : public PageManager {
public:
  ConcreteTestPageManager() : PageManager(4096) {}
  std::unique_ptr<PageRef> getPage(const PageManager::Page& page) override {
    return std::unique_ptr<PageRef>(nullptr);
  }
  void fsync() const override {}
  size_t endPos() const { return end_pos_; }
};

class TestMmapPageManager : public MmapPageManager {
public:
  explicit TestMmapPageManager(
      int fd,
      size_t len,
      size_t block_size) :
      MmapPageManager(fd, len, block_size) {}

  MmappedFile* getMmappedFileTest(uint64_t last_byte) {
    return getMmappedFile(last_byte);
  }
};

TEST_CASE(PageManagerTest, TestAbstractPageManagerAlloc, [] () {
    ConcreteTestPageManager page_manager;

    auto page1 = page_manager.allocPage(3000);
    EXPECT_EQ(page_manager.endPos(), 4096);
    EXPECT_EQ(page1.offset, 0);
    EXPECT_EQ(page1.size, 4096);
    page_manager.freePage(page1);
});

TEST_CASE(PageManagerTest, TestAbstractPageManagerAllocMulti, [] () {
    ConcreteTestPageManager page_manager;

    auto page1 = page_manager.allocPage(3000);
    EXPECT_EQ(page_manager.endPos(), 4096);
    EXPECT_EQ(page1.offset, 0);
    EXPECT_EQ(page1.size, 4096);
    page_manager.freePage(page1);

    auto page2 = page_manager.allocPage(8000);
    EXPECT_EQ(page_manager.endPos(), 12288);
    EXPECT_EQ(page2.offset, 4096);
    EXPECT_EQ(page2.size, 8192);

    auto page3 = page_manager.allocPage(3000);
    EXPECT_EQ(page3.offset, 0);
    EXPECT_EQ(page3.size, 4096);
});

TEST_CASE(PageManagerTest, TestAbstractPageManagerAllocFree, [] () {
    ConcreteTestPageManager page_manager;

    auto page1 = page_manager.allocPage(3000);
    EXPECT_EQ(page_manager.endPos(), 4096);
    EXPECT_EQ(page1.offset, 0);
    EXPECT_EQ(page1.size, 4096);
    page_manager.freePage(page1);

    auto page2 = page_manager.allocPage(8000);
    EXPECT_EQ(page_manager.endPos(), 12288);
    EXPECT_EQ(page2.offset, 4096);
    EXPECT_EQ(page2.size, 8192);

    auto page3 = page_manager.allocPage(3000);
    EXPECT_EQ(page3.offset, 0);
    EXPECT_EQ(page3.size, 4096);
    page_manager.freePage(page2);

    auto page4 = page_manager.allocPage(4000);
    EXPECT_EQ(page_manager.endPos(), 12288);
    EXPECT_EQ(page4.offset, 4096);
    EXPECT_EQ(page4.size, 8192);
});

TEST_CASE(PageManagerTest, TestMmapPageManager, [] () {
  int fd = open("/tmp/__fnordmetric_testMmapPageManager",
      O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
  EXPECT(fd > 0);
  unlink("/tmp/__fnordmetric_testMmapPageManager");
  auto page_manager = new TestMmapPageManager(fd, 0, 4096);

  auto mfile1 = page_manager->getMmappedFileTest(3000);
  auto mfile2 = page_manager->getMmappedFileTest(304200);
  EXPECT_EQ(mfile1->size, 1048576);
  EXPECT_EQ(mfile1, mfile2);
  mfile2->incrRefs();

  auto mfile3 = page_manager->getMmappedFileTest(1048577);
  EXPECT_EQ(mfile3->size, 1048576 * 2);
  EXPECT(mfile3 != mfile2);
  mfile2->decrRefs();

  delete page_manager;
  close(fd);
});

