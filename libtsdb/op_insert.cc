/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "tsdb.h"
#include "page_index.h"

namespace tsdb {

bool TSDB::insertUInt64(
    uint64_t series_id,
    uint64_t time,
    uint64_t value) {
  Transaction txn;
  if (!txn_map_.startTransaction(series_id, false, &txn)) {
    return false;
  }

  return true;
}

} // namespace tsdb

