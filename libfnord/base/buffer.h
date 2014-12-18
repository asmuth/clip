/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_UTIL_BUFFER_H_
#define _FNORD_UTIL_BUFFER_H_
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {

class Buffer {
public:
  static const size_t npos = -1;

  Buffer();
  Buffer(const void* initial_data, size_t initial_size);
  Buffer(size_t initial_size);
  Buffer(const Buffer& copy);
  Buffer(Buffer&& move);
  Buffer& operator=(const Buffer& copy) = delete;
  Buffer& operator=(Buffer&& move);
  ~Buffer();

  bool operator==(const Buffer& buf) const;
  bool operator==(const char* str) const;
  bool operator==(const std::string& str) const;

  void append(const void* data, size_t size);
  void clear();
  void truncate(size_t size);

  void* data() const;
  char charAt(size_t pos) const;
  size_t find(char chr) const;
  size_t size() const;
  size_t allocSize() const;
  std::string toString() const;

  void reserve(size_t size);

  void setMark(size_t mark);
  size_t mark() const;

protected:
  void* data_;
  size_t size_;
  size_t alloc_;
  size_t mark_;
};

}
#endif
