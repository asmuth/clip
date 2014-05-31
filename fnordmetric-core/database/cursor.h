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

namespace fnordmetric {
namespace database {

class StreamRef;

/**
 * A storage cursor is a stateful iterator for a single stream. It can be used
 * to read rows from and append rows to the stream.
 */
class Cursor {
public:
  explicit Cursor(std::shared_ptr<StreamRef> stream_ref);
  Cursor(const Cursor& copy) = delete;
  Cursor& operator=(const Cursor& copy) = delete;

  /**
   * Seek to the first row in the stream with an insert time later or equal to
   * position. Position must be a UTC millisecond timestamp.
   *
   * Returns the UTC millisecond timestamp of the row the cursor is pointing to
   * after the seek operation.
   */
  uint64_t seekTo(uint64_t position);

  /**
   * Seek to the first row in the stream.
   *
   * Returns the UTC millisecond timestamp of the row the cursor is pointing to
   * after the seek operation.
   */
  uint64_t seekToFirst();

  /**
   * Seek to the last row in the stream.
   *
   * Returns the UTC millisecond timestamp of the row the cursor is pointing to
   * after the seek operation.
   */
  uint64_t seekToLast();

  /**
   * Try to advance the cursor by one row. Returns true if the cursor was
   * advanced successfully and returns false if the end of stream was reached.
   */
  bool next();

  /**
   * Get the row the cursor is currently pointing to. If the cursor points to a
   * valid row this method will call the provided callack function with these
   * arguments:
   *   - data: the pointer to the rows data
   *   - len: the rows length in bytes
   *   - time: the rows insert time in UTC milliseconds
   *
   * Note that the data pointer is only valid within the callback function's
   * scope! The reason why this takes a callback function rather than returning
   * a pointer to the data directly is that this allows us to return a reference
   * to the internal storage of the data. If this method would return a direct
   * pointer to the data all we'd be forced to copy the data before returning
   * if we ever want to free our internal storage again (let aside smart pointer
   * trickery or an explicit free mechanism that would needlessly complicate
   * things)
   */
  void getRow(const std::function<void (const uint8_t* data,
      size_t len, uint64_t time)>& func) const;

protected:
  std::shared_ptr<StreamRef> stream_ref_;
};

}
}
#endif
