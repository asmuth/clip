/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnord/base/buffer.h>
#include <fnord/base/exception.h>
#include <string.h>

namespace fnord {

Buffer::Buffer() : data_(nullptr), size_(0) {}

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

Buffer::Buffer(const Buffer& copy) : size_(copy.size_) {
  data_ = malloc(size_);

  if (data_ == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }

  memcpy(data_, copy.data_, size_);
}

Buffer::Buffer(Buffer&& move) : data_(move.data_), size_(move.size_) {
  move.data_ = nullptr;
  move.size_ = 0;
}

Buffer& Buffer::operator=(Buffer&& move) {
  data_ = move.data_;
  size_ = move.size_;
  move.data_ = nullptr;
  move.size_ = 0;
  return *this;
}

Buffer::~Buffer() {
  if (data_ != nullptr) {
    free(data_);
  }
}

void Buffer::append(const void* data, size_t size) {
  if (data_ == nullptr) {
    size_ = 0;
    data_ = malloc(size);
  } else {
    data_ = realloc(data_, size_ + size);
  }

  if (data_ == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }

  memcpy((char*) data_ + size_, data, size);
}

void Buffer::clear() {
  size_ = 0;

  if (data_ != nullptr) {
    free(data_);
  }

  data_ = nullptr;
}

void* Buffer::data() const {
  return data_;
}

char Buffer::charAt(size_t pos) const {
  if (pos >= size_) {
    RAISE(kIndexError, "index out of bounds");
  }

  return static_cast<char *>(data_)[pos];
}

size_t Buffer::size() const {
  return size_;
}

std::string Buffer::toString() const {
  return std::string(static_cast<char *>(data_), size_);
}

}

