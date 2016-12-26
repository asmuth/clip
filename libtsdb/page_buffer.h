/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <stdlib.h>
#include <algorithm>
#include <vector>

namespace tsdb {

enum class PageType : uint64_t { UINT64 = 1 };

class PageBuffer {
public:

  PageBuffer(PageType type);
  PageBuffer(const PageBuffer& o) = delete;
  PageBuffer(PageBuffer&& o) ;
  PageBuffer& operator=(const PageBuffer& o);
  PageBuffer& operator=(PageBuffer&& o);
  ~PageBuffer();

  void insert(uint64_t time, const void* value, size_t value_len);
  void update(size_t pos, const void* value, size_t value_len);

  void getTimestamp(size_t pos, uint64_t* timestamp) const;
  void getValue(size_t pos, uint64_t* value) const;

  size_t getSize() const;

  void encode(std::string* out) const;
  bool decode(const char* data, size_t len);

protected:

  using ValueVectorUInt64Type = std::vector<uint64_t>;

  static constexpr const size_t kValuesVectorSize =
      sizeof(ValueVectorUInt64Type);

  PageType type_;
  std::vector<uint64_t> timestamps_;
  char values_[kValuesVectorSize];
};

} // namespace tsdb

