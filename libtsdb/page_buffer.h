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

class PageBuffer {
public:

  PageBuffer();
  PageBuffer(size_t value_size);
  PageBuffer(const PageBuffer& o) = delete;
  PageBuffer(PageBuffer&& o) ;
  PageBuffer& operator=(const PageBuffer& o);
  PageBuffer& operator=(PageBuffer&& o);
  ~PageBuffer();

  void update(size_t pos, const void* value, size_t value_len);
  void insert(size_t pos, uint64_t time, const void* value, size_t value_len);
  void append(uint64_t time, const void* value, size_t value_len);

  void getTimestamp(size_t pos, uint64_t* timestamp) const;
  void getValue(size_t pos, void* value, size_t value_len) const;

  size_t getSize() const;

  void encode(std::string* out) const;
  bool decode(const char* data, size_t len);

protected:

  using ValueVectorUInt64Type = std::vector<uint64_t>;

  size_t value_size_;
  std::vector<uint64_t> timestamps_;
  std::vector<std::string> values_;
};

} // namespace tsdb

