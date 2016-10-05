/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>

namespace tsdb {

struct PageIndexEntry {
  size_t page_id;
};

struct PageIndexSplitpoint {
  uint64_t point;
};

class PageIndex {
public:

  PageIndex();
  PageIndex(const PageIndex& o) = delete;
  PageIndex& operator=(const PageIndex& o) = delete;
  ~PageIndex();

  bool alloc(size_t nentries);

  PageIndexEntry* getEntries();
  const PageIndexEntry* getEntries() const;

  size_t getSize() const;

protected:
  PageIndexEntry* entries_;
  PageIndexSplitpoint* splitpoints_;
  size_t size_;
};

} // namespace tsdb

