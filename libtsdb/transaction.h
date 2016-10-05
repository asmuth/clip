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
#include <mutex>
#include <map>

namespace tsdb {
class PageIndex;
struct TransactionSnapshot;
struct TransactionContext;

class Transaction {
public:

  Transaction();
  Transaction(bool rdonly, TransactionContext* ctx, TransactionSnapshot* snap);
  Transaction(Transaction&& o);
  Transaction(const Transaction& o) = delete;
  Transaction& operator=(const Transaction& o) = delete;
  Transaction& operator=(Transaction&& o);
  ~Transaction();

  const PageIndex* getPageIndex() const;
  void setPageIndex(std::unique_ptr<PageIndex>&& page_index);

  void close();

protected:
  bool readonly_;
  TransactionContext* ctx_;
  TransactionSnapshot* snap_;
};

class TransactionMap {
public:

  TransactionMap();
  ~TransactionMap();

  bool startTransaction(
      uint64_t slot_id,
      bool readonly,
      Transaction* txn);

  bool createSlot(
      uint64_t slot_id,
      std::unique_ptr<PageIndex>&& page_index);

protected:
  std::map<uint64_t, TransactionContext*> slots_; // FIXME allow limit to uint32_t in build config
  std::mutex mutex_;
};

} // namespace tsdb

