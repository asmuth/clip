/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "btree.h"

namespace fnordmetric {

BTree::BTree(uint16_t branch_factor) :
    branch_factor_(branch_factor),
    root_is_leaf_(true) {
  root_ = allocNode(getLeafNodeSize());
}

void BTree::insert(uint64_t key) {
  if (root_is_leaf_ && root_->used < branch_factor_) {
    return insertIntoNode(root_, key);
  }
}

BTree::LeafNodeEntry* BTree::lookup(uint64_t key) const {
  if (root_is_leaf_) {
    return (LeafNodeEntry*) root_->leaf;
  }
}

BTree::Node* BTree::allocNode(size_t size) {
  Node* node = (Node *) malloc(size); // FIXPAUL
  memset(node, 0, size);
  return node;
}

void BTree::insertIntoNode(Node* leaf_node, uint64_t key) {
  auto slot = leaf_node->leaf + leaf_node->used;
  slot->key = key;
  leaf_node->used++;
}

}
