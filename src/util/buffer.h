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
namespace util {

class Buffer {
public:

  Buffer(void* initial_data, size_t initial_size);
  Buffer(size_t initial_size);
  Buffer(Buffer&& move);
  Buffer(const Buffer& copy) = delete;
  Buffer& operator=(const Buffer& copy) = delete;
  ~Buffer();

  void* data() const;
  size_t size() const;

protected:
  void* data_;
  size_t size_;
};

}
}
#endif
