/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_BTREE_H
#define _FNORDMETRIC_BTREE_H
#include <stdint.h>

namespace fnordmetric {

/**
 * All methods on a collection instance are threadsafe.
 */
class BTree {
  friend class BTreeTest;
public:

  static const uint16_t kBranchFactor = 128;

  BTree(uint16_t branch_factor);

  void insert(uint64_t key);

protected:

  struct __attribute__((__packed__)) LeafNodeEntry {
    uint64_t key;
    uint64_t offset;
    uint32_t size;
  };

  struct __attribute__((__packed__)) InternalNodeEntry {
    uint64_t key;
    uint64_t offset;
    uint8_t  is_leaf;
  };

  struct __attribute__((__packed__)) Node {
    uint16_t used;
    union __attribute__((__packed__)) {
      LeafNodeEntry leaf[];
      InternalNodeEntry internal[];
    };
  };

  inline size_t getLeafNodeSize() {
    return sizeof(Node) + sizeof(LeafNodeEntry) * branch_factor_;
  }

  inline size_t getInternalNodeSize() {
    return sizeof(Node) + sizeof(InternalNodeEntry) * branch_factor_;
  }


  LeafNodeEntry* lookup(uint64_t key) const;
  Node* allocNode(size_t size);

  void insertIntoNode(Node* leaf_node, uint64_t key);

  const uint16_t branch_factor_;
  bool root_is_leaf_;
  Node* root_;
};

}
#endif
