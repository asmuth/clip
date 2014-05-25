/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "filebackend.h"
#include "pagemanager.h"

namespace fnordmetric {
namespace filebackend {

class FileBackendTest {
public:
  FileBackendTest() {}

  void run() {
    testStreamIdAssignment();
    testStreamRefCreation();
    testPageManager();
  }

  void testStreamIdAssignment() {
    FileBackend backend;
    std::string key1 = "83d2f71c457206bf-Ia9f37ed7-F76b77d1a";
    std::string key2 = "83d2f71c457216bf-Ia9f37ed7-F76b77d1a";

    assert(backend.getStreamId(key1) == 1);
    assert(backend.getStreamId(key2) == 2);
    assert(backend.getStreamId(key1) == 1);
    assert(backend.getStreamId(key2) == 2);
  }

  void testStreamRefCreation() {
    FileBackend backend;
    std::string key1 = "83d2f71c457206bf-Ia9f37ed7-F76b77d1a";
    std::string key2 = "83d2f71c457216bf-Ia9f37ed7-F76b77d1a";
    auto ref1 = backend.getStreamRef(key1);
    assert(ref1.get() != nullptr);
    auto ref2 = backend.getStreamRef(key1);
    assert(ref1.get() == ref2.get());
    auto ref3 = backend.getStreamRef(key2);
    assert(ref1.get() != ref3.get());
    assert(ref2.get() != ref3.get());
    auto ref4 = backend.getStreamRef(key2);
    assert(ref1.get() != ref4.get());
    assert(ref2.get() != ref4.get());
    assert(ref3.get() == ref4.get());
  }

  void testPageManager() {
    PageManager page_manager(0, 4096);

    auto page1 = page_manager.getPage(3000);
    printf("page size: %llu\n", page1.size);
    assert(page_manager.end_pos_ == 4096);
    assert(page1.offset == 0);
    assert(page1.size == 4096);
    assert(page1.used == 0);
    page_manager.yieldPage(page1);

    auto page2 = page_manager.getPage(8000);
    assert(page_manager.end_pos_ == 12288);
    assert(page2.offset == 4096);
    assert(page2.size == 8192);
    assert(page2.used == 0);

    auto page3 = page_manager.getPage(3000);
    assert(page3.offset == 0);
    assert(page3.size == 4096);
    assert(page3.used == 0);

    page_manager.yieldPage(page2);
    auto page4 = page_manager.getPage(4000);
    assert(page_manager.end_pos_ == 12288);
    assert(page4.offset == 4096);
    assert(page4.size == 8192);
    assert(page4.used == 0);
  }

};

}
}

int main() {
  fnordmetric::filebackend::FileBackendTest test;
  test.run();
  printf("all tests passed! :)\n");
}
