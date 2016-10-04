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

struct PageMapEntry {
  bool is_dirty;
  union {
    struct dirty {
      void* page_buffer_;
    };
    struct cold {
      uint64_t page_addr_; // FIXME allow limit to uint32_t in build config
      uint64_t page_size_; // FIXME allow limit to uint32 in build config
    };
  } data;
};

struct PageMapSplitpoint {
  uint64_t point;
};

class PageMap {
public:

  PageMap();
  PageMap(const PageMap& o) = delete;
  PageMap& operator=(const PageMap& o) = delete;
  ~PageMap();

protected:
  PageMapEntry* entries_;
  PageMapSplitpoint* splitpoints_;
  size_t size_;
};

} // namespace tsdb

