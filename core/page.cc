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
#include <stdexcept>
#include "tuple.h"
#include "metadata.h"

namespace zdb {

template <typename T>
void page_buf_fixed<T>::append(const void* val, size_t val_len) {
  assert(val_len == sizeof(T));
  data.emplace_back(*static_cast<const T*>(val));
}

page_buf* page_malloc(zdb_type_t type) {
  switch (type) {
    case ZDB_BOOL: return new page_buf_bool();
    case ZDB_INT32: return new page_buf_int32();
    case ZDB_INT64: return new page_buf_int64();
    case ZDB_UINT32: return new page_buf_uint32();
    case ZDB_UINT64: return new page_buf_uint64();
    case ZDB_FLOAT32: return new page_buf_float32();
    case ZDB_FLOAT64: return new page_buf_float64();
  }
  
  throw std::runtime_error("invalid type");
}

} // namespace zdb

