/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_RADIXTREE_H
#define _FNORD_RADIXTREE_H
#include "3rdparty/libart/art.h"

namespace fnord {

template <typename T>
class RadixTree {
public:
  static_assert(
      sizeof(T) <= sizeof(void*),
      "RadixTree<T>: sizeof(T) must be <= sizeof(void*)");

  RadixTree();
  ~RadixTree();

  void insert(const String& key, T value);

  Option<T> get(const String& key) const;
  uint64_t size() const;

protected:
  art_tree tree_;
};

}

#include "RadixTree_impl.h"
#endif
