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

Buffer::Buffer() : data_(nullptr), size_(0), alloc_(0), mark_(0) {}

Buffer::Buffer(
    const void* initial_data,
    size_t initial_size) :
    data_(malloc(initial_size)),
    size_(initial_size),
    alloc_(initial_size),
    mark_(0) {
  if (data_ == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }

  memcpy(data_, initial_data, size_);
}

Buffer::Buffer(
    size_t initial_size) :
    data_(malloc(initial_size)),
    size_(initial_size),
    alloc_(initial_size),
    mark_(0) {
  if (data_ == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }
}

Buffer::Buffer(const Buffer& copy) : size_(copy.size_), alloc_(copy.size_) {
  data_ = malloc(size_);

  if (data_ == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }

  memcpy(data_, copy.data_, size_);
}

Buffer& Buffer::operator=(const Buffer& copy) {
  if (data_ != nullptr) {
    free(data_);
  }

  size_ = copy.size_;
  alloc_ = copy.size_;
  data_ = malloc(alloc_);

  if (data_ == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }

  memcpy(data_, copy.data_, size_);
  return *this;
}

Buffer::Buffer(
    Buffer&& move) :
    data_(move.data_),
    size_(move.size_),
    alloc_(move.alloc_),
    mark_(0) {
  move.data_ = nullptr;
  move.size_ = 0;
  move.alloc_ = 0;
}

Buffer& Buffer::operator=(Buffer&& move) {
  data_ = move.data_;
  size_ = move.size_;
  alloc_ = move.alloc_;
  mark_ = move.mark_;
  move.data_ = nullptr;
  move.size_ = 0;
  move.alloc_ = 0;
  move.mark_ = 0;
  return *this;
}

Buffer::~Buffer() {
  if (data_ != nullptr) {
    free(data_);
  }
}

bool Buffer::operator==(const char* str) const {
  if (strlen(str) != size_) {
    return false;
  }

  return memcmp(data_, str, size_) == 0;
}

void Buffer::append(const void* data, size_t size) {
  if (size_ + size > alloc_) {
    reserve((size_ + size) - alloc_);
  }

  memcpy((char*) data_ + size_, data, size);
  size_ += size;
}

void Buffer::append(const String& string) {
  append(string.data(), string.length());
}

void Buffer::truncate(size_t size) {
  if (size > size_) {
    RAISE(kIndexError, "requested size is out of bounds");
  }

  size_ = size;
}

void Buffer::reserve(size_t size) {
  alloc_ += size;

  if (data_ == nullptr) {
    data_ = malloc(alloc_);
  } else {
    data_ = realloc(data_, alloc_);
  }

  if (data_ == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }
}

void Buffer::clear() {
  size_ = 0;
  mark_ = 0;
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

size_t Buffer::find(char chr) const {
  for (size_t n = 0; n < size_; ++n) {
    if (((char *) data_)[n] == chr) {
      return n;
    }
  }

  return Buffer::npos;
}

size_t Buffer::size() const {
  return size_;
}

size_t Buffer::allocSize() const {
  return alloc_;
}

std::string Buffer::toString() const {
  return std::string(static_cast<char *>(data_), size_);
}

void Buffer::setMark(size_t mark) {
  mark_ = mark;
}

size_t Buffer::mark() const {
  return mark_;
}

}

