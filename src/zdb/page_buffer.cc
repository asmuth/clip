/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include "page_buffer.h"
#include "varint.h"

namespace tsdb {

PageBuffer::PageBuffer() : value_size_(0) {}

PageBuffer::PageBuffer(size_t value_size) : value_size_(value_size) {}

PageBuffer::PageBuffer(
    PageBuffer&& o) :
    value_size_(o.value_size_),
    timestamps_(std::move(o.timestamps_)),
    values_(std::move(o.values_)) {}

PageBuffer& PageBuffer::operator=(const PageBuffer& o) {
  value_size_ = o.value_size_;
  timestamps_ = o.timestamps_;
  values_ = o.values_;
  return *this;
}

PageBuffer& PageBuffer::operator=(PageBuffer&& o) {
  value_size_ = o.value_size_;
  timestamps_ = std::move(o.timestamps_);
  values_ = std::move(o.values_);
  return *this;
}

PageBuffer::~PageBuffer() {}

void PageBuffer::insert(
    size_t pos,
    uint64_t time,
    const void* value,
    size_t value_len) {
  assert(value_len == value_size_);
  assert(pos <= values_.size());
  timestamps_.insert(timestamps_.begin() + pos, time);
  values_.insert(
      values_.begin() + pos,
      std::string((const char*) value, value_len));
}

void PageBuffer::append(uint64_t time, const void* value, size_t value_len) {
  assert(value_len == value_size_);
  timestamps_.push_back(time);
  values_.push_back(std::string((const char*) value, value_len));
}

void PageBuffer::update(size_t pos, const void* value, size_t value_len) {
  assert(value_len == value_size_);
  assert(pos <= values_.size());
  values_[pos] = std::string((const char*) value, value_len);
}

void PageBuffer::getTimestamp(size_t pos, uint64_t* timestamp) const {
  assert(pos < timestamps_.size());
  *timestamp = timestamps_[pos];
}

void PageBuffer::getValue(size_t pos, void* value, size_t value_len) const {
  assert(pos < values_.size());
  assert(value_len == value_size_);
  assert(values_[pos].size() == value_size_);
  memcpy(value, values_[pos].data(), value_size_);
}

size_t PageBuffer::getSize() const {
  return timestamps_.size();
}

void PageBuffer::encode(std::string* out) const {
  writeVarUInt(out, timestamps_.size());
  for (const auto& t : timestamps_) {
    writeVarUInt(out, t);
  }
  for (const auto& v : values_) {
    assert(v.size() == value_size_);
    *out += v;
  }
}

bool PageBuffer::decode(const char* data, size_t len) {
  assert(value_size_ > 0);
  auto cur = data;
  auto end = cur + len;

  uint64_t nentries;
  if (!readVarUInt(&cur, end, &nentries)) {
    return false;
  }

  timestamps_.resize(nentries);
  for (uint64_t i = 0; i < nentries; ++i) {
    if (!readVarUInt(&cur, end, &timestamps_[i])) {
      return false;
    }
  }

  values_.resize(nentries);
  for (uint64_t i = 0; i < nentries; ++i) {
    if (cur + value_size_ >= end) {
      return false;
    }

    values_[i] = std::string(cur, value_size_);
    cur += value_size_;
  }

  return true;
}

} // namespace tsdb

