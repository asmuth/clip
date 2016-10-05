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
#include <vector>

namespace tsdb {

enum class PageType { UINT64 };

class PageBuffer {
public:

  void insert(uint64_t time, const void* value, size_t value_len);

protected:
  std::vector<uint64_t> timestamps;
  union values {
    std::vector<uint64_t> uint64;
  };
};

} // namespace tsdb

