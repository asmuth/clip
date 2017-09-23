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
#include <string>
#include "page_buffer.h"

namespace tsdb {

struct PageIndexEntry {
  size_t page_id;
};

struct PageIndexSplitpoint {
  uint64_t point;
};

class PageIndex {
public:

  PageIndex(uint64_t value_size, const std::string metadata);
  PageIndex(const PageIndex& o) = delete;
  PageIndex& operator=(const PageIndex& o) = delete;
  ~PageIndex();

  bool alloc(size_t nentries);

  PageIndexEntry* getEntries();
  const PageIndexEntry* getEntries() const;

  PageIndexSplitpoint* getSplitpoints();
  const PageIndexSplitpoint* getSplitpoints() const;

  size_t getSize() const;
  size_t getValueSize() const;

  bool hasDiskSnapshot() const;
  void getDiskSnapshot(uint64_t* addr, uint64_t* size) const;
  void setDiskSnapshot(uint64_t addr, uint64_t size);

  const std::string& getMetadata() const;

protected:
  uint64_t value_size_;
  std::string metadata_;
  PageIndexEntry* entries_;
  PageIndexSplitpoint* splitpoints_;
  size_t size_;
  uint64_t disk_addr_;
  uint64_t disk_size_;
};

} // namespace tsdb

