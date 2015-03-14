/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
namespace fnord {

template <typename T>
RadixTree<T>::RadixTree() {
  if (art_tree_init(&tree_) != 0) {
    RAISE(kRuntimeError, "art_tree_init() failed");
  }
}

template <typename T>
RadixTree<T>::~RadixTree() {
  art_tree_destroy(&tree_);
}

template <typename T>
void RadixTree<T>::insert(const String& key, T value) {
  art_insert(
      &tree_,
      (unsigned char *) key.c_str(),
      key.length(),
      (void*) value);
}

template <typename T>
Option<T> RadixTree<T>::get(const String& key) const {
  auto res = art_search(
      &tree_,
      (unsigned char *) key.c_str(),
      key.length());

  if (res == nullptr) {
    return None<T>();
  } else {
    return Some((T) res);
  }
}

template <typename T>
uint64_t RadixTree<T>::size() const {
  return art_size(&tree_);
}

}
