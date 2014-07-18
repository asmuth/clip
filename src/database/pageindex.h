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
class DocumentRef;

/*
struct StreamPosition {
  uint64_t unix_millis;
  uint64_t logical_offset;
  uint64_t next_offset;
  bool operator==(const StreamPosition& other);
};
*/
/**
 * This should be a linked list of smaller index blocks so that the COW overhead
 * is fixed...
 *
 * Pages should be removed via deletePage() and then a compact() (which rebuilds
 * the list into a two node list with one short node holding the most recent
 * pages for fast committing and one long node holding all older pages
 */
class PageIndex {
  friend class DatabaseTest;
public:
  PageIndex(std::shared_ptr<PageManager> page_manager);

  PageIndex(
      std::shared_ptr<PageManager> page_manager,
      const PageManager::Page& index_page,
      uint32_t used);

  struct __attribute__((__packed__)) IndexPageEntry {
    uint64_t offset;
    uint64_t first_key;
    uint32_t size;
    uint32_t used;
  };

  static const size_t kInitialIndexPageSize = sizeof(IndexPageEntry) * 4;

  /**
   * Target page size in number of rows. Default: 16384 rows
   */
  static size_t kTargetRowsPerPage;

  PageIndex(const PageIndex& copy) = delete;
  PageIndex& operator=(const PageIndex& copy) = delete;
  ~PageIndex();

  /**
   * Access the StreamRefs internal page storage (do not call this method unless
   * you know what you are doing)
   */
  //void accessPages(std::function<void(
  //    const std::vector<std::shared_ptr<IndexPageEntry>>&)> func);

  IndexPageEntry* getIndexPageEntryForInsert(size_t bytes);

  PageIndex* clone();

  size_t getNumPages() const;

protected:

  uint32_t used_;

  IndexPageEntry* appendPage(const PageManager::Page& page);

  const std::shared_ptr<PageManager> page_manager_;
  PageManager::Page index_page_;
  std::unique_ptr<PageManager::PageRef> index_page_ref_;
};

}
#endif
