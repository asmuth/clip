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
#include "transaction.h"
#include "page_map.h"

namespace tsdb {

template <class T>
static void addReference(T* ptr) {
  ptr->refcount_.fetch_add(1);
}

template <class T>
static void dropReference(T* ptr) {
  if (std::atomic_fetch_sub_explicit(
          &ptr->refcount_,
          size_t(1),
          std::memory_order_release) == 1) {
    std::atomic_thread_fence(std::memory_order_acquire);
    delete ptr;
  }
}

struct TransactionSnapshot {
  TransactionSnapshot();
  std::unique_ptr<PageMap> page_map_;
  std::atomic<size_t> refcount_;
};

TransactionSnapshot::TransactionSnapshot() :
    refcount_(1) {}

struct TransactionContext {
  TransactionContext();
  ~TransactionContext();
  std::mutex write_lock_;
  std::atomic<TransactionSnapshot*> snapshot_;
  std::atomic<size_t> refcount_;
};

TransactionContext::TransactionContext() :
    snapshot_(new TransactionSnapshot()),
    refcount_(1) {}

TransactionContext::~TransactionContext() {
  dropReference(snapshot_.load());
}

TransactionMap::TransactionMap() : slot_id_(0) {}

Transaction::Transaction() : readonly_(true), ctx_(nullptr), snap_(nullptr) {}

Transaction::Transaction(
    bool readonly,
    TransactionContext* context,
    TransactionSnapshot* snap) :
    readonly_(readonly),
    ctx_(context),
    snap_(snap) {}

Transaction::Transaction(Transaction&& o) :
    readonly_(o.readonly_),
    ctx_(o.ctx_),
    snap_(o.snap_) {
  o.readonly_ = true;
  o.ctx_ = nullptr;
  o.snap_ = nullptr;
}

Transaction& Transaction::operator=(Transaction&& o) {
  close();

  readonly_ = o.readonly_;
  ctx_ = o.ctx_;
  snap_ = o.snap_;

  o.readonly_ = true;
  o.ctx_ = nullptr;
  o.snap_ = nullptr;

  return *this;
}

Transaction::~Transaction() {
  close();
}

PageMap* Transaction::getPageMap() {
  return snap_->page_map_.get();
}

void Transaction::setPageMap(std::unique_ptr<PageMap>&& page_map) {
  assert(!readonly_);
  assert(ctx_->snapshot_.load() == snap_);

  auto old_snap = snap_;
  auto new_snap = new TransactionSnapshot();
  new_snap->page_map_ = std::move(page_map);

  snap_ = new_snap;
  ctx_->snapshot_ = new_snap;
  dropReference(old_snap);
}

void Transaction::close() {
  if (!snap_) {
    return;
  }

  if (!readonly_) {
    ctx_->write_lock_.unlock();
  }

  readonly_ = true;
  dropReference(snap_);
  snap_ = nullptr;
  dropReference(ctx_);
  ctx_ = nullptr;
}

bool TransactionMap::startTransaction(
    uint64_t slot_id,
    bool readonly,
    Transaction* txn) {
  /* grab the lock and search for the slot */
  std::unique_lock<std::mutex> lk(mutex_);
  auto iter = slots_.find(slot_id);
  if (iter == slots_.end()) {
    return false;
  }

  /* make a copy of the slot pointer and increment the refcount, so the slot
     pointer will stay valid after we unlock the mutex */
  auto txn_ctx = iter->second;
  addReference(txn_ctx);

  /* unlock the mutex - no further access to the slot map */
  lk.unlock();

  /* if this is a writeable transaction, pause until we can aquire the write
     lock */
  if (!readonly) {
    txn_ctx->write_lock_.lock();
  }

  /* make a copy of the current snapshot and increment the snapshot refcount */
  auto txn_snap = txn_ctx->snapshot_.load();
  addReference(txn_snap);

  *txn = Transaction(readonly, txn_ctx, txn_snap);
  return true;
}

uint64_t TransactionMap::createSlot() {
  std::unique_lock<std::mutex> lk(mutex_);
  slots_.emplace(++slot_id_, new TransactionContext());
  return slot_id_;
}

} // namespace tsdb

