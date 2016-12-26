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
#include <vector>
#include <algorithm>
#include "page_buffer.h"
#include "varint.h"

namespace tsdb {

PageBuffer::PageBuffer(PageType type) : type_(type) {
  switch (type_) {
    case PageType::UINT64:
      new (values_) ValueVectorUInt64Type();
      break;
    default:
      assert(false);
  }
}

PageBuffer::PageBuffer(
    PageBuffer&& o) :
    type_(o.type_),
    timestamps_(std::move(o.timestamps_)) {
  switch (type_) {
    case PageType::UINT64:
      new (values_) ValueVectorUInt64Type(
          std::move(*((ValueVectorUInt64Type*) o.values_)));
      break;
    default:
      assert(false);
  }
}

PageBuffer& PageBuffer::operator=(const PageBuffer& o) {
  assert(type_ == o.type_);
  timestamps_ = o.timestamps_;
  switch (type_) {
    case PageType::UINT64:
      *((ValueVectorUInt64Type*) values_) =
          *((ValueVectorUInt64Type*) o.values_);
      break;
    default:
      assert(false);
  }

  return *this;
}

PageBuffer& PageBuffer::operator=(PageBuffer&& o) {
  assert(type_ == o.type_);
  timestamps_ = std::move(o.timestamps_);
  switch (type_) {
    case PageType::UINT64:
      *((ValueVectorUInt64Type*) values_) =
          std::move(*((ValueVectorUInt64Type*) o.values_));
      break;
    default:
      assert(false);
  }

  return *this;
}

PageBuffer::~PageBuffer() {
  switch (type_) {
    case PageType::UINT64:
      ((ValueVectorUInt64Type*) values_)->~vector();
      break;
    default:
      assert(false);
  }
}

template <typename ValueVectorType, typename ValueType>
static void insertValue(
    ValueVectorType* value_vector,
    size_t pos,
    const ValueType& value) {
  assert(pos <= value_vector->size());
  value_vector->insert(value_vector->begin() + pos, value);
}

void PageBuffer::insert(size_t pos, uint64_t time, const void* value, size_t value_len) {
  timestamps_.insert(timestamps_.begin() + pos, time);

  switch (type_) {
    case PageType::UINT64:
      insertValue((ValueVectorUInt64Type*) values_, pos, *((uint64_t*) value));
      break;
  }
}

template <typename ValueVectorType, typename ValueType>
static void appendValue(
    ValueVectorType* value_vector,
    const ValueType& value) {
  value_vector->push_back(value);
}

void PageBuffer::append(uint64_t time, const void* value, size_t value_len) {
  timestamps_.push_back(time);

  switch (type_) {
    case PageType::UINT64:
      appendValue((ValueVectorUInt64Type*) values_, *((uint64_t*) value));
      break;
  }
}

template <typename ValueVectorType, typename ValueType>
static void updateValue(
    ValueVectorType* value_vector,
    size_t pos,
    const ValueType& value) {
  assert(pos <= value_vector->size());
  (*value_vector)[pos] = value;
}

void PageBuffer::update(size_t pos, const void* value, size_t value_len) {
  switch (type_) {
    case PageType::UINT64:
      updateValue((ValueVectorUInt64Type*) values_, pos, *((uint64_t*) value));
      break;
  }
}

void PageBuffer::getTimestamp(size_t pos, uint64_t* timestamp) const {
  assert(pos < timestamps_.size());
  *timestamp = timestamps_[pos];
}

void PageBuffer::getValue(size_t pos, uint64_t* value) const {
  assert(type_ == PageType::UINT64);
  auto& values = *((ValueVectorUInt64Type*) values_);
  assert(pos < values.size());
  *value = values[pos];
}

size_t PageBuffer::getSize() const {
  return timestamps_.size();
}

void PageBuffer::encode(std::string* out) const {
  writeVarUInt(out, timestamps_.size());
  for (const auto& t : timestamps_) {
    writeVarUInt(out, t);
  }
  auto& values = *((ValueVectorUInt64Type*) values_);
  for (const auto& v : values) {
    writeVarUInt(out, v);
  }
}

bool PageBuffer::decode(const char* data, size_t len) {
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

  auto& values = *((ValueVectorUInt64Type*) values_);
  values.resize(nentries);
  for (uint64_t i = 0; i < nentries; ++i) {
    if (!readVarUInt(&cur, end, &values[i])) {
      return false;
    }
  }

  return true;
}

} // namespace tsdb

