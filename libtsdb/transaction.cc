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
#include <set>
#include "transaction.h"
#include "page_index.h"
#include "page_map.h"

namespace tsdb {

template <class T>
static void addReference(T* ptr) {
  ptr->refcount_.fetch_add(1);
}

struct TransactionSnapshot {
  TransactionSnapshot(PageMap* page_map, uint64_t version);
  ~TransactionSnapshot();
  PageMap* page_map_;
  std::unique_ptr<PageIndex> page_index_;
  std::atomic<size_t> refcount_;
  std::atomic<TransactionSnapshot*> next_;
  std::set<PageMap::PageIDType> dropped_pages_;
  const uint64_t version_;
};

TransactionSnapshot::TransactionSnapshot(
    PageMap* page_map,
    uint64_t version) :
    page_map_(page_map),
    refcount_(1),
    next_(nullptr),
    version_(version) {}

TransactionSnapshot::~TransactionSnapshot() {
  for (const auto& p : dropped_pages_) {
    page_map_->deletePage(p);
  }
}

static void dropSnapshotReference(TransactionSnapshot* ptr) {
  while (ptr) {
    if (std::atomic_fetch_sub_explicit(
            &ptr->refcount_,
            size_t(1),
            std::memory_order_release) == 1) {
      std::atomic_thread_fence(std::memory_order_acquire);
      auto next_ptr = ptr->next_.load();
      delete ptr;
      ptr = next_ptr;
      continue;
    } else {
      break;
    }
  }
}

struct TransactionContext {
  TransactionContext(PageMap* page_map);
  ~TransactionContext();
  std::mutex write_lock_;
  std::atomic<TransactionSnapshot*> snapshot_;
  std::atomic<size_t> refcount_;
};

TransactionContext::TransactionContext(
    PageMap* page_map) :
    snapshot_(new TransactionSnapshot(page_map, 1)),
    refcount_(1) {}

TransactionContext::~TransactionContext() {
  dropSnapshotReference(snapshot_.load());
}

static void dropContextReference(TransactionContext* ptr) {
  if (std::atomic_fetch_sub_explicit(
          &ptr->refcount_,
          size_t(1),
          std::memory_order_release) == 1) {
    std::atomic_thread_fence(std::memory_order_acquire);
    delete ptr;
  }
}

TransactionMap::TransactionMap(PageMap* page_map) : page_map_(page_map) {}

TransactionMap::~TransactionMap() {
  for (auto& s : slots_) {
    dropContextReference(s.second);
    s.second = nullptr;
  }
}

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

const PageIndex* Transaction::getPageIndex() const {
  return snap_->page_index_.get();
}

void Transaction::updatePageIndex(
    std::unique_ptr<PageIndex>&& page_index,
    const std::set<PageMap::PageIDType>& deleted_pages /* = {} */) {
  assert(!readonly_);
  assert(ctx_->snapshot_.load() == snap_);

  auto old_snap = snap_;
  auto new_snap = new TransactionSnapshot(
      old_snap->page_map_,
      old_snap->version_ + 1);

  new_snap->page_index_ = std::move(page_index);
  new_snap->refcount_.fetch_add(1);
  old_snap->dropped_pages_ = deleted_pages;
  old_snap->next_ = new_snap;

  std::atomic_thread_fence(std::memory_order_acq_rel);

  snap_ = new_snap;
  ctx_->snapshot_ = new_snap;
  dropSnapshotReference(old_snap);
}

uint64_t Transaction::getVersion() const {
  if (!snap_) {
    return 0;
  }

  return snap_->version_;
}

void Transaction::close() {
  if (!snap_) {
    return;
  }

  if (!readonly_) {
    ctx_->write_lock_.unlock();
  }

  readonly_ = true;
  dropSnapshotReference(snap_);
  snap_ = nullptr;
  dropContextReference(ctx_);
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

bool TransactionMap::createSlot(
    uint64_t slot_id,
    std::unique_ptr<PageIndex>&& page_index) {
  std::unique_lock<std::mutex> lk(mutex_);
  if (slots_.count(slot_id) > 0) {
    return false;
  }

  auto txn_ctx = new TransactionContext(page_map_);
  txn_ctx->snapshot_.load()->page_index_ = std::move(page_index);

  slots_.emplace(slot_id, txn_ctx);
  return true;
}

void TransactionMap::listSlots(std::set<uint64_t>* slots) {
  std::unique_lock<std::mutex> lk(mutex_);
  for (const auto& s : slots_) {
    slots->emplace(s.first);
  }
}

} // namespace tsdb

