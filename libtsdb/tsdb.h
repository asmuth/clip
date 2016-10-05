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
#include "transaction.h"
#include "page_map.h"

namespace tsdb {

class TSDB {
public:

  TSDB();
  TSDB(const TSDB& o) = delete;
  TSDB& operator=(const TSDB& o) = delete;
  ~TSDB();

  bool createSeries(uint64_t series_id);

  bool insertUInt64(
      uint64_t series_id,
      uint64_t time,
      uint64_t value);

protected:
  TransactionMap txn_map_;
  PageMap page_map_;
};

} // namespace tsdb

