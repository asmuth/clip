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
#include "pagemanager.h"

namespace fnordmetric {
namespace database {

class Database;
class Cursor;

/**
 * A stream descriptor is a handle to a single stream. It can be used to
 * append rows to the stream and to receive a cursor for reading from the
 * stream.
 */
class StreamRef {
  friend class DatabaseTest;
public:
  // FIXPAUL reader writer lock on page alloc?
  struct PageAlloc {
    PageManager::Page page;
    size_t used; /* number of used bytes in the page */
    uint64_t time; /* time of the first row in the page */
  };

  struct __attribute__((__packed__)) RowHeader {
    uint32_t checksum;
    uint32_t size;
    uint64_t time;
    uint8_t data[];
    void computeChecksum();
  };

  explicit StreamRef(
      Database* backed,
      uint64_t stream_id,
      const std::string& stream_key);

  explicit StreamRef(
      Database* backed,
      uint64_t stream_id,
      const std::string& stream_key,
      std::vector<PageAlloc>&& pages);

  StreamRef(const StreamRef& copy) = delete;
  StreamRef& operator=(const StreamRef& copy) = delete;

  /**
   * Append a new row to the very end of the opened stream. Returns the UTC 
   * millisecond timestamp at which the row was inserted.
   */
  uint64_t appendRow(const std::vector<uint8_t>& data);

  /**
   * Return a cursor to this stream for reading. The initial position of the
   * cursor is undefined.
   */
  std::unique_ptr<Cursor> getCursor();

protected:
  std::vector<PageAlloc> pages_;
  Database* backend_;
  const uint64_t stream_id_;
  const std::string stream_key_;
};

}
}
#endif
