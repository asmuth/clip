/**
 * Copyright (c) 2016, Paul Asmuth <paul@asmuth.com>
 * All rights reserved.
 * 
 * This file is part of the "libzdb" project. libzdb is free software licensed
 * under the 3-Clause BSD License (BSD-3-Clause).
 */
#pragma once
#include "tuple.h"
#include "metadata.h"

namespace zdb {

struct page_info {
};

class page_buf {
public:
  virtual ~page_buf() = default;
  virtual void append(const void* val, size_t val_len) = 0;
};

template <typename T>
class page_buf_fixed : public page_buf {
public:
  void append(const void* val, size_t val_len) override;
protected:
  std::vector<T> data;
};

using page_buf_bool = page_buf_fixed<bool>;
using page_buf_int64 = page_buf_fixed<int64_t>;
using page_buf_int32 = page_buf_fixed<int32_t>;
using page_buf_uint64 = page_buf_fixed<uint64_t>;
using page_buf_uint32 = page_buf_fixed<uint32_t>;
using page_buf_float32 = page_buf_fixed<float>;
using page_buf_float64 = page_buf_fixed<double>;

page_buf* page_malloc(zdb_type_t type);

} // namespace zdb

