/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_SSTABLE_INDEXPROVIDER_H
#define _FNORD_SSTABLE_INDEXPROVIDER_H
#include <fnord-sstable/index.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>

namespace fnord {
namespace sstable {

class IndexProvider {
public:

  IndexProvider();
  IndexProvider(IndexProvider&& other);
  IndexProvider(const IndexProvider& copy) = delete;
  IndexProvider& operator=(const IndexProvider& copy) = delete;

  template <typename IndexType>
  void addIndex();

  std::vector<Index::IndexRef>&& popIndexes();

protected:
  std::vector<Index::IndexRef> indexes_;
};


template <typename IndexType>
void IndexProvider::addIndex() {
  indexes_.emplace_back(IndexType::makeIndex());
}

}
}

#endif
