/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include "fnordmetric/util/stdtypes.h"
#include "fnordmetric/util/autoref.h"

/**
 * A growing/growable buffer class.
 *
 * A buffer has both a "capacity" and a "logical size" used to describe the
 * number of used bytes in the buffer and the size of the backing memory
 * allocation respectively. The capacity is always greater or equal to the
 * logical size.
 *
 * A buffer is always backed by one contiguous chunk of memory that is
 * dynamically alloced/realloced as needed. The backing memory allocation is
 * "capacity" bytes large and contains logical size bytes of data (stored as one
 * contiguous block starting at offset 0)
 *
 * The append methods will append new data to the end of the buffer, i.e. after
 * the Nth byte where N = logical size, increasing the logical size and
 * allocating new memory as needed.
 *
 * You can also retrieve a mutable pointer to the backing storage by calling
 * the data() method and write directly into the backing storage.
 *
 * In order to keep this class simple and predictable it does _not_ try to be
 * smart about memory allocations, ie. it will not pre-allocate anything and
 * won't optimistically preallocate more memory than required to store it's
 * contents unless you tell it to.
 *
 * This means writing directly to a buffer in small increments without calling
 * reserve first can be slow as it will incur a lot of memory allocations.
 */
class Buffer : public RefCounted {
public:
  static const size_t npos = -1;

  /**
   * Allocate a new, empty buffer with size=0 and capaity=0
   */
  Buffer();

  /**
   * Allocate a new buffer with size = initial_size and capacity = initial_size
   * and fill it by copying initial_size bytes from initial_data into the
   * backing storage
   */
  Buffer(const void* initial_data, size_t initial_size);

  /**
   * Allocate a new buffer with size = initial_size and capacity = initial_size
   * but do not initialize the memory. The contents of the newly created buffer
   * are undefined.
   */
  Buffer(size_t initial_size);

  /**
   * Allocate a new buffer with size = string.size and capacity = string.size
   * and fill it by copying string.size bytes from the provided string into the
   * backing storage
   */
  Buffer(const String& string);

  /**
   * Allocate a new buffer with the same logical size as the provided buffer
   * and capacity = logical size (!) and fill it by copying logical_size bytes
   * from the provided buffer into backing storage
   *
   * N.B. That this copy constructor does _not_ preserve the capacity of the
   * source buffer
   */
  Buffer(const Buffer& copy);

  /**
   * Move the buffer without copying data or allocating new memory (this will
   * acutally move the backing memory allocation so it is very cheap).
   *
   * This operation will preserve both the size _and_ the capacity of the buffer
   */
  Buffer(Buffer&& move);

  Buffer& operator=(const Buffer& copy);
  Buffer& operator=(Buffer&& move);

  ~Buffer();

  bool operator==(const Buffer& buf) const;
  bool operator==(const char* str) const;
  bool operator==(const std::string& str) const;

  /**
   * Append data to this buffer. The buffer logical size will be increased by
   * "size" bytes. If the buffer capacity is less than then new logical size it
   * will be increasd to exactly the new logical size (incurring a
   * malloc/realloc). If the buffer capacity is greater than or equal to the new
   * logical size it will be unchanged an no memory allocation will occur.
   */
  void append(const void* data, size_t size);
  void append(const String& string);
  void append(const Buffer& buffer);
  void append(char chr);

  /**
   * Clear this buffer by setting the logical size to zero. Does _not_ free any
   * memory, zero out the backing storage or change the capacity of the buffer
   */
  void clear();

  void truncate(size_t size);

  /**
   * Retrieve a mutable pointer to the backing storage. The pointed to memory
   * chunk is capacity bytes large and contains logical size bytes of data
   * (stored as one contiguous chunk starting at offset 0)
   */
  void* data() const;
  char charAt(size_t pos) const;
  size_t find(char chr) const;

  /**
   * Return the logical size of the buffer
   */
  size_t size() const;

  /**
   * Sets the logical size for this buffer.
   *
   * If the new logical size is larger than the current capacity of the buffer,
   * memory will be allocated and the capacity will be increased to the new
   * logical size. The previous contents are unchanged but the value of any
   * newly allocated bytes is undefined.
   *
   * If the new logical size is less than the currenct capacity of the buffer,
   * the capacity and contents of the buffer will be unchanged and no memoy
   * will be allocated or freed
   */
  void resize(size_t size);

  /**
   * Return the actual size of the backing malloc, this may be larger than the
   * value returned by size
   */
  size_t allocSize() const;
  size_t capacity() const;

  /**
   * Return the number of bytes that can be appended to this buffer without
   * requiring a new memory allocation. (remaining = capacity - logical size)
   */
  size_t remaining() const;

  /**
   * Reserve "size" new bytes of memory for future use. This method will not
   * change the logical size of the buffer (as returned by a call to size) but
   * only increase the capacity by size bytes.
   */
  void reserve(size_t size);

  std::string toString() const;

  void setMark(size_t mark);
  size_t mark() const;

  template <typename T>
  inline T* structAt(size_t pos) const {
    return (T*) (((char *) data()) + pos);
  }

protected:
  void* data_;
  size_t size_;
  size_t alloc_;
  size_t mark_;
};

typedef RefPtr<Buffer> BufferRef;

