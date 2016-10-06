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
#include <atomic>
#include <memory>
#include <map>
#include <mutex>
#include "page_buffer.h"

namespace tsdb {

struct PageInfo {
  uint64_t version;
  bool is_dirty;
  uint64_t disk_addr;
  uint64_t disk_size;
};

class PageMap {
public:

  using PageIDType = size_t;

  PageMap(int fd);
  PageMap(const PageMap& o) = delete;
  PageMap& operator=(const PageMap& o) = delete;
  ~PageMap();

  PageIDType allocPage(PageType type);

  PageIDType addColdPage(PageType type, uint64_t disk_addr, uint64_t disk_size);

  bool getPageInfo(PageIDType page_id, PageInfo* info);

  bool getPage(PageIDType page_id, PageBuffer* buf);

  bool modifyPage(
      PageType page_type,
      PageIDType page_id,
      std::function<bool (PageBuffer* buf)> fn);

  void flushPage(
      PageIDType page_id,
      uint64_t version,
      uint64_t disk_addr,
      uint64_t disk_size);

  void deletePage(PageIDType page_id);

protected:

  struct PageMapEntry {
    std::unique_ptr<PageBuffer> buffer;
    std::mutex lock;
    uint64_t version;
    uint64_t disk_addr;
    uint64_t disk_size;
    std::atomic<size_t> refcount;
  };

  void dropEntryReference(PageMap::PageMapEntry* entry);

  bool loadPage(
      uint64_t disk_addr,
      uint64_t disk_size,
      PageBuffer* buffer);

  int fd_;
  std::mutex mutex_;
  PageIDType page_id_;
  std::map<PageIDType, PageMapEntry*> map_;
};

} // namespace tsdb

