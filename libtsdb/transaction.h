/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <mutex>
#include <map>

namespace tsdb {
class PartitionMap;
struct TransactionSnapshot;
struct TransactionContext;

class Transaction {
public:

  Transaction();
  Transaction(TransactionContext* slot);
  Transaction(Transaction&& o);
  Transaction(const Transaction& o) = delete;
  Transaction& operator=(const Transaction& o) = delete;
  Transaction& operator=(Transaction&& o);
  ~Transaction();

protected:
  bool valid_;
  TransactionContext* slot_;
  TransactionSnapshot* snapshot_;
};

class TransactionMap {
public:

  TransactionMap();
  ~TransactionMap();

  bool startTransaction(
      uint64_t slot_id,
      uint64_t flags,
      Transaction* txn);

  uint64_t createSlot();

protected:
  std::map<uint64_t, TransactionContext*> slots_;
  uint64_t slot_id_;
  std::mutex mutex_;
};

} // namespace tsdb

