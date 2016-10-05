/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "page_map.h"

namespace tsdb {

PageMap::PageMap() : page_id_(0) {}

PageMap::~PageMap() {
  for (auto& e : map_) {
    dropEntryReference(e.second);
  }
}

size_t PageMap::allocPage(PageType type) {
  auto entry = new PageMapEntry();
  entry->refcount = 1;
  entry->buffer.reset(new PageBuffer(type));

  std::unique_lock<std::mutex> lk(mutex_);

  auto page_id = ++page_id_;
  map_.emplace(page_id, entry);

  return page_id;
}

bool PageMap::loadPage(
    PageIDType page_id,
    PageBuffer* buf) {
  /* grab the main mutex and locate the page in our map */
  std::unique_lock<std::mutex> map_lk(mutex_);
  auto iter = map_.find(page_id);
  if (iter == map_.end()) {
    return false;
  }

  /* copy the page */
  *buf = *iter->second->buffer;
  return true;
}

bool PageMap::modifyPage(
    PageIDType page_id,
    std::function<bool (PageBuffer* buf)> fn) {
  /* grab the main mutex and locate the page in our map */
  std::unique_lock<std::mutex> map_lk(mutex_);
  auto iter = map_.find(page_id);
  if (iter == map_.end()) {
    return false;
  }

  /* increment the entries refcount and unlock the main mutex */
  auto entry = iter->second;
  entry->refcount.fetch_add(1);
  map_lk.unlock();

  /* grab the entries lock and perform the modification */
  std::unique_lock<std::mutex> entry_lk(entry->lock);
  auto rc = fn(entry->buffer.get());

  /* decrement the entries refcount and return */
  dropEntryReference(entry);
  return rc;
}

void PageMap::dropEntryReference(PageMap::PageMapEntry* entry) {
  if (std::atomic_fetch_sub_explicit(
          &entry->refcount,
          size_t(1),
          std::memory_order_release) == 1) {
    std::atomic_thread_fence(std::memory_order_acquire);
    delete entry;
  }
}

} // namespace tsdb

