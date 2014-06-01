/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_FILEBACKEND_CURSOR_H
#define _FNORDMETRIC_FILEBACKEND_CURSOR_H
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <memory>
#include <functional>
#include "pagemanager.h"

namespace fnordmetric {
namespace database {

class StreamRef;
struct PageAlloc;
struct RowHeader;
struct StreamPosition;

/**
 * A storage cursor is a stateful iterator for a single stream. It can be used
 * to read rows from the stream.
 *
 * A cursor object is not threadsafe! If you want to use a cursor from multiple
 * threads you must take care to synchronize access in such a way that no two
 * threads call any method on the cursor object at the same time!
 */
class Cursor {
public:
  explicit Cursor(
      StreamRef* stream_ref,
      std::shared_ptr<PageManager> page_manager);

  Cursor(const Cursor& copy) = delete;
  Cursor& operator=(const Cursor& copy) = delete;

  /**
   * Seek to the first row in the stream with an insert time later or equal to
   * position. Position must be a UTC millisecond timestamp.
   *
   * Returns the position of the cursor after the seek operation.
   */
  StreamPosition seekToTime(uint64_t unix_millis);

  /**
   * Seek to the first row in the stream with a logical offset larger than or
   * equal to offset.
   *
   * Returns the position of the cursor after the seek operation.
   */
  StreamPosition seekToLogicalOffset(uint64_t logical_offset);

  /**
   * Seek to the first row in the stream.
   *
   * Returns the position of the cursor after the seek operation.
   */
  StreamPosition seekToFirst();

  /**
   * Seek to the last row in the stream.
   *
   * Returns the position of the cursor after the seek operation.
   */
  StreamPosition seekToLast();

  /**
   * Try to advance the cursor by one row. Returns true if the cursor was
   * advanced successfully and returns false if the end of stream was reached.
   */
  bool next();

  /**
   * Get the row the cursor is currently pointing to. If the cursor points to a
   * valid row this method will return a pointer to that row.
   *
   * The returned pointer is valid until the cursor is destroyed or any seek
   * or next method is called on the cursor!
   */
  const RowHeader* getCurrentRow() const;

  /**
   * Get the current stream position
   */
  StreamPosition getCurrentPosition() const;

protected:
  StreamRef* stream_ref_;
  std::shared_ptr<const PageAlloc> current_page_;
  std::unique_ptr<PageManager::PageRef> current_page_ref_;
  uint64_t current_page_offset_;
  size_t current_page_index_;
  std::shared_ptr<PageManager> page_manager_;
};

}
}
#endif
