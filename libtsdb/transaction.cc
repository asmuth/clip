/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "transaction.h"

namespace tsdb {

struct TransactionSnapshot {
  std::unique_ptr<PartitionMap> partition_map;
  std::atomic<size_t> refcount_;
};

struct TransactionContext {
  TransactionContext();
  //~TransactionContext();
  std::mutex write_lock_;
  std::atomic<TransactionSnapshot*> snapshot_;
  std::atomic<size_t> refcount_;
};

TransactionContext::TransactionContext() : refcount_(1) {}

TransactionMap::TransactionMap() : slot_id_(0) {}

bool TransactionMap::startTransaction(
    uint64_t slot_id,
    uint64_t flags,
    Transaction* txn) {
  std::unique_lock<std::mutex> lk(mutex_);
  auto iter = slots_.find(slot_id);
  if (iter == slots_.end()) {
    return false;
  }

  auto txn_ctx = iter->second;
  txn_ctx->refcount_.fetch_add(1);
  lk.unlock();

  *txn = Transaction(txn_ctx);
  return true;
}

uint64_t TransactionMap::createSlot() {
  std::unique_lock<std::mutex> lk(mutex_);

  auto new_slot_id = ++slot_id_;
  auto new_txn = new TransactionContext();

  slots_.emplace(new_slot_id, new_txn);
  return new_slot_id;
}

} // namespace tsdb

