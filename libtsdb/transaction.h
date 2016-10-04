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
class PageMap;
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

  PageMap* getPageMap();
  void setPageMap(std::unique_ptr<PageMap>&& page_map);

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

  uint64_t createSlot();

protected:
  std::map<uint64_t, TransactionContext*> slots_; // FIXME allow limit to uint32_t in build config
  uint64_t slot_id_; // FIXME allow limit to uint32_t in build config
  std::mutex mutex_;
};

} // namespace tsdb

