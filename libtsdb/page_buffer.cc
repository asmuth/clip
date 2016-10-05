/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "page_buffer.h"

namespace tsdb {

PageBuffer::PageBuffer(PageType type) : type_(type) {
  switch (type_) {
    case PageType::UINT64:
      new (values_) ValueVectorUInt64Type();
      break;
  }
}

PageBuffer::~PageBuffer() {
  switch (type_) {
    case PageType::UINT64:
      ((ValueVectorUInt64Type*) values_)->~vector();
      break;
  }
}

template <typename ValueVectorType, typename ValueType>
static void insertValue(
    ValueVectorType* value_vector,
    size_t pos,
    const ValueType& value) {
  value_vector->insert(value_vector->begin() + pos, value);
}

void PageBuffer::insert(uint64_t time, const void* value, size_t value_len) {
  auto timestamp_iter = std::lower_bound(
      timestamps_.begin(),
      timestamps_.end(),
      time);

  auto pos = timestamp_iter - timestamps_.begin();
  timestamps_.insert(timestamp_iter, time);

  switch (type_) {
    case PageType::UINT64:
      insertValue((ValueVectorUInt64Type*) values_, pos, *((uint64_t*) value));
      break;
  }
}

} // namespace tsdb

