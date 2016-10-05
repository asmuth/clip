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

class Cursor {
public:

  Cursor();
  Cursor(Transaction&& txn);
  Cursor(Cursor&& o);
  Cursor(const Cursor& o) = delete;
  Cursor& operator=(const Cursor& o) = delete;
  Cursor& operator=(Cursor&& o);

  bool next(uint64_t* timestamp, uint64_t* value);

protected:
  Transaction txn_;
};

} // namespace tsdb

