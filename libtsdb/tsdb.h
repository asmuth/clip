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

namespace tsdb {

class TSDB {
public:

  TSDB();
  TSDB(const TSDB& o) = delete;
  TSDB& operator=(const TSDB& o) = delete;
  ~TSDB();

  bool createSeries(uint64_t series_id);

protected:
  TransactionMap txn_map_;
};

} // namespace tsdb

