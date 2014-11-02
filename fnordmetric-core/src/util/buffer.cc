/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/buffer.h>
#include <fnordmetric/util/runtimeexception.h>
#include <string.h>

namespace fnord {
namespace util {

Buffer::Buffer(
    const void* initial_data,
    size_t initial_size) :
    data_(malloc(initial_size)),
    size_(initial_size) {
  if (data_ == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }

  memcpy(data_, initial_data, size_);
}

Buffer::Buffer(
    size_t initial_size) :
    data_(malloc(initial_size)),
    size_(initial_size) {
  if (data_ == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }
}

Buffer::Buffer(Buffer&& move) : data_(move.data_), size_(move.size_) {
  move.data_ = nullptr;
  move.size_ = 0;
}

Buffer::~Buffer() {
  if (data_ != nullptr) {
    free(data_);
  }
}

void* Buffer::data() const {
  return data_;
}

size_t Buffer::size() const {
  return size_;
}


}
}

