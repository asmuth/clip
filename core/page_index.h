/**
 * Copyright (c) 2016, Paul Asmuth <paul@asmuth.com>
 * All rights reserved.
 * 
 * This file is part of the "libzdb" project. libzdb is free software licensed
 * under the 3-Clause BSD License (BSD-3-Clause).
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

