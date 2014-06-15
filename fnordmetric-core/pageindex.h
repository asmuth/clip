/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_PAGEINDEX_H
#define _FNORDMETRIC_PAGEINDEX_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include "pagemanager.h"

namespace fnordmetric {

class Database;
class Cursor;

struct PageAlloc {
  PageAlloc(
      const PageManager::Page& page,
      uint64_t first_key);
  const PageManager::Page page_;
  uint64_t used_; /* number of used bytes in the page */
  const uint64_t first_key_; /* key of the first document in the page */
};

/*
struct StreamPosition {
  uint64_t unix_millis;
  uint64_t logical_offset;
  uint64_t next_offset;
  bool operator==(const StreamPosition& other);
};
*/

struct __attribute__((__packed__)) RowHeader {
  uint32_t checksum;
  uint32_t size;
  uint64_t time;
  uint8_t data[];
  uint32_t computeChecksum();
};

class PageIndex {
  friend class DatabaseTest;
public:
  PageIndex(const PageManager::Page& index_page) {}
  PageIndex(const PageIndex& copy) = delete;
  PageIndex& operator=(const PageIndex& copy) = delete;

  /**
   * Access the StreamRefs internal page storage (do not call this method unless
   * you know what you are doing)
   */
  void accessPages(std::function<void(
      const std::vector<std::shared_ptr<PageAlloc>>&)> func);

protected:

  uint64_t estimatePageSize(size_t last_page_avg_size, size_t row_size) const;

  // this is suboptimal as it will force us to do random memory accesses when
  // trying to binary search over the pages first row times
  std::vector<std::shared_ptr<PageAlloc>> pages_;
  size_t num_pages_;
};

}
#endif
