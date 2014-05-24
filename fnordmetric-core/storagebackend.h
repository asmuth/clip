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
#include <tuple>

namespace fnordmetric {

class IStorageCursor {
public:

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
   * Read up to N rows from the stream. Advances the cursor by the number of rows
   * that were read an returns the number of rows that were read. Returns 0 if
   * the end of the stream was reached.
   *
   * The method returns tuples where the first item is the UTC millisecond
   * timestamp at which the row was written and the second item is the row as
   * a binary string.
   */
  virtual size_t read(
    size_t n,
    std::vector<std::tuple<uint64_t, const std::vector<uint8_t>>>* destination) = 0;

  /**
   * Append a new row to the very end of the currenly opened stream
   *
   * Returns the UTC millisecond timestamp at which the row was inserted.
   */
  virtual uint64_t appendRow(const std::vector<uint8_t>& data) = 0;

};

class IStorageBackend {
public:

  IStorageBackend() {}
  IStorageBackend(const IStorageBackend& copy) = delete;
  virtual ~IStorageBackend() {}

  virtual std::unique_ptr<IStorageCursor> getCursor(const std::string& key) = 0;

};

}

#endif
