/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "cursor.h"

namespace tsdb {

Cursor::Cursor() {}

Cursor::Cursor(Transaction&& txn) : txn_(std::move(txn)) {}

Cursor::Cursor(Cursor&& o) : txn_(std::move(o.txn_)) {}

Cursor& Cursor::operator=(Cursor&& o) {
  txn_ = std::move(o.txn_);
  return *this;
}

//bool next(uint64_t* timestamp, uint64_t* value);

} // namespace tsdb

