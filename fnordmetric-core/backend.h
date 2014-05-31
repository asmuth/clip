/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_BACKEND_H
#define _FNORDMETRIC_BACKEND_H

#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <functional>

namespace fnordmetric {

class IBackend {
public:
  class IStreamDescriptor;
  class IStreamCursor;

  /**
   * Open or create the stream with the specified key
   */
  virtual std::unique_ptr<IStreamDescriptor> openStream(
      const std::string& key) = 0;

  /**
   * A stream descriptor is a handle to a single stream. It can be used to
   * append rows to the stream and to receive a cursor for reading from the
   * stream.
   */
  class IStreamDescriptor {
  public:

    /**
     * Append a new row to the very end of the opened stream. Returns the UTC 
     * millisecond timestamp at which the row was inserted.
     */
    virtual uint64_t appendRow(const std::vector<uint8_t>& data) = 0;

    /**
     * Return a cursor to this stream for reading. The initial position of the
     * cursor is undefined.
     */
    virtual std::unique_ptr<IStreamCursor> getCursor() = 0;

    IStreamDescriptor() {}
    IStreamDescriptor(const IStreamDescriptor& copy) = delete;
    IStreamDescriptor& operator=(const IStreamDescriptor& copy) = delete;
    virtual ~IStreamDescriptor() {}
  };

  /**
   * A storage cursor is a stateful iterator for a single stream. It can be used
   * to read rows from and append rows to the stream.
   */
  class IStreamCursor {
  public:
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
     * Try to advance the cursor by one row. Returns true if the cursor was
     * advanced successfully and returns false if the end of stream was reached.
     */
    virtual bool next() = 0;

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
    virtual void getRow(const std::function<void (const uint8_t* data,
        size_t len, uint64_t time)>& func) const = 0;

    IStreamCursor() {}
    IStreamCursor(const IStreamCursor& copy) = delete;
    IStreamCursor& operator=(const IStreamCursor& copy) = delete;
    virtual ~IStreamCursor() {}
  };

  IBackend() {}
  IBackend(const IBackend& copy) = delete;
  IBackend& operator=(const IBackend& copy) = delete;
  virtual ~IBackend() {}
};

}

#endif
