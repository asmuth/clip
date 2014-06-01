/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_FILEBACKEND_STREAMREF_H
#define _FNORDMETRIC_FILEBACKEND_STREAMREF_H

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include <atomic>
#include "pagemanager.h"
#include "../record.h"

namespace fnordmetric {
namespace database {

class Database;
class Cursor;

// FIXPAUL reader writer lock on page alloc?
struct PageAlloc {
  PageAlloc(const PageManager::Page& page, uint64_t time);
  const PageManager::Page page_;
  std::atomic_size_t used_; /* number of used bytes in the page */
  const uint64_t time_; /* time of the first row in the page */
};

struct __attribute__((__packed__)) RowHeader {
  uint32_t checksum;
  uint32_t size;
  uint64_t time;
  uint8_t data[];
  uint32_t computeChecksum();
};

/**
 * A stream descriptor is a handle to a single stream. It can be used to
 * append rows to the stream and to receive a cursor for reading from the
 * stream.
 */
class StreamRef {
  friend class DatabaseTest;
  friend class Cursor;
public:
  explicit StreamRef(
      Database* backed,
      uint64_t stream_id,
      const std::string& stream_key);

  explicit StreamRef(
      Database* backed,
      uint64_t stream_id,
      const std::string& stream_key,
      std::vector<std::shared_ptr<PageAlloc>>&& pages);

  StreamRef(const StreamRef& copy) = delete;
  StreamRef& operator=(const StreamRef& copy) = delete;

  /**
   * Append a new row to the very end of the opened stream. Returns the UTC
   * millisecond timestamp at which the row was inserted.
   */
  uint64_t appendRow(const RecordWriter& row);

  /**
   * Return a cursor to this stream for reading. The initial position of the
   * cursor is undefined.
   */
  std::unique_ptr<Cursor> getCursor();

protected:
  uint64_t appendRow(const void* data, size_t size);
  // this is suboptimal as it will force us to do random memory accesses when
  // trying to binary search over the pages first row times
  std::vector<std::shared_ptr<PageAlloc>> pages_;
  std::atomic_size_t num_pages_;
  Database* backend_;
  const uint64_t stream_id_;
  const std::string stream_key_;
};

}
}
#endif
