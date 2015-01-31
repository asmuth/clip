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
#include "btree.h"

namespace fnordmetric {

class BTreeTest {
public:
  BTreeTest() {}

  void run() {
    testInsertIntoRoot();
    testRootPageSplit();
  }

  void testInsertIntoRoot() {
    uint16_t b = 128;
    BTree btree(b);
    auto root_node = btree.root_;
    assert(root_node != nullptr);
    for (int i=0; i < b-1; ++i) {
      btree.insert(i);
    }
    assert(btree.root_->used == b - 1);
    for (int i=0; i < b-1; ++i) {
      assert(btree.root_->leaf[i].key == i);
    }
    assert(btree.root_ == root_node);
  };

  void testRootPageSplit() {
    uint16_t b = 128;
    BTree btree(b);
    auto root_node = btree.root_;
    assert(root_node != nullptr);
    for (int i=0; i < (b-1) * 2; ++i) {
      btree.insert(i);
    }
    assert(btree.root_ != root_node);
    assert(btree.root_->used == 2);
  };

};

}

int main() {
  fnordmetric::BTreeTest test;
  test.run();
  printf("all tests passed! :)\n");
}
