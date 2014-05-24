/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_STORAGEBACKEND_H
#define _FNORDMETRIC_STORAGEBACKEND_H

#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <functional>

namespace fnordmetric {

class IStorageCursor;

/**
 * A storage backend stores an arbitrary number of 'streams'. A stream consists
 * of rows. Each row is a <time, data> tuple where time is the time at which the
 * row was inserted into the stream and data is a binary string. Streams are
 * append only. Each stream is identified by a unique string key.
 */
class IStorageBackend {
public:
  IStorageBackend(const IStorageBackend& copy) = delete;
  IStorageBackend& operator=(const IStorageBackend& copy) = delete;
  virtual ~IStorageBackend() {}

  /**
   * Get a cursor for a stream with a specified key
   */
  virtual std::unique_ptr<IStorageCursor> getCursor(const std::string& key) = 0;

protected:
  IStorageBackend() {}
};


/**
 * A storage cursor is a stateful iterator for a single stream. It can be used
 * to read rows from and append rows to the stream.
 */
class IStorageCursor {
public:
  typedef std::pair<uint64_t, const std::vector<uint8_t>> RowType;

  IStorageCursor() {}
  virtual ~IStorageCursor() {}

  /**
   * Seek to the first row in the stream with an insert time later or equal to
   * position. Position must be a UTC millisecond timestamp.
   *
   * Returns the UTC millisecond timestamp of the row the cursor is pointing to
   * after the seek operation.
   */
  virtual uint64_t seekTo(uint64_t position) = 0;

  /**
   * Seek to the first row in the stream.
   *
   * Returns the UTC millisecond timestamp of the row the cursor is pointing to
   * after the seek operation.
   */
  virtual uint64_t seekToFirst() = 0;

  /**
   * Seek to the last row in the stream.
   *
   * Returns the UTC millisecond timestamp of the row the cursor is pointing to
   * after the seek operation.
   */
  virtual uint64_t seekToLast() = 0;

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
   * a pointer to the data directly is that this allows implementations to
   * return a reference to the internal storage of the data. If this method
   * would retunr a direct pointer all implementations would be forced to copy
   * the data before returning if they ever wanted to free their internal
   * storage again.
   */
  virtual void getRow(const std::function<void (const uint8_t* data,
      size_t len, uint64_t time)>& func) const = 0;

  /**
   * Try to advance the cursor by one row. Returns true if the cursor was
   * advanced successfully and returns false if the end of stream was reached.
   */
  virtual bool next() = 0;

  /**
   * Append a new row to the very end of the currenly opened stream. This will
   * seek to the end of the stream and then write a row.
   *
   * Returns the UTC millisecond timestamp at which the row was inserted.
   */
  virtual uint64_t appendRow(const std::vector<uint8_t>& data) = 0;

  /**
   * Returns a clone of this storage cursor
   */
  virtual std::unique_ptr<IStorageCursor> clone() const = 0;

};

}

#endif
