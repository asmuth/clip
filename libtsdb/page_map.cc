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
#include <unistd.h>
#include "page_map.h"

namespace tsdb {

PageMap::PageMap(int fd) : fd_(fd), page_id_(0) {}

PageMap::~PageMap() {
  for (auto& e : map_) {
    dropEntryReference(e.second);
  }
}

PageMap::PageIDType PageMap::allocPage(PageType type) {
  auto entry = new PageMapEntry();
  entry->refcount = 1;
  entry->buffer.reset(new PageBuffer(type));
  entry->version = 1;
  entry->disk_addr = 0;
  entry->disk_size = 0;

  std::unique_lock<std::mutex> lk(mutex_);

  auto page_id = ++page_id_;
  map_.emplace(page_id, entry);

  return page_id;
}

PageMap::PageIDType PageMap::addColdPage(
    PageType type,
    uint64_t disk_addr,
    uint64_t disk_size) {
  auto entry = new PageMapEntry();
  entry->refcount = 1;
  entry->version = 1;
  entry->disk_addr = disk_addr;
  entry->disk_size = disk_size;

  std::unique_lock<std::mutex> lk(mutex_);

  auto page_id = ++page_id_;
  map_.emplace(page_id, entry);

  return page_id;
}

bool PageMap::getPageInfo(PageIDType page_id, PageInfo* info) {
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

  /* grab the entries lock and copy the info */
  std::unique_lock<std::mutex> entry_lk(entry->lock);
  info->version = entry->version;
  info->is_dirty = !!entry->buffer;
  info->disk_addr = entry->disk_addr;
  info->disk_size = entry->disk_size;
  entry_lk.unlock();

  /* decrement the entries refcount and return */
  dropEntryReference(entry);
  return true;
}

bool PageMap::getPage(
    PageIDType page_id,
    PageBuffer* buf) {
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

  /* grab the pages lock */
  std::unique_lock<std::mutex> entry_lk(entry->lock);

  /* if the page is buffered in memory, copy and return */
  if (entry->buffer) {
    *buf = *entry->buffer;
    entry_lk.unlock();
    dropEntryReference(entry);
    return true;
  }

  /* load the page from disk */
  auto disk_addr = entry->disk_addr;
  auto disk_size = entry->disk_size;
  entry_lk.unlock();
  dropEntryReference(entry);
  return loadPage(disk_addr, disk_size, buf);
}

bool PageMap::modifyPage(
    PageType page_type,
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

  /* grab the entries lock  */
  std::unique_lock<std::mutex> entry_lk(entry->lock);

  /* if the page is not in memory, load it */
  if (!entry->buffer) {
    entry->buffer.reset(new PageBuffer(page_type));
    if (!loadPage(entry->disk_addr, entry->disk_size, entry->buffer.get())) {
      entry->buffer.reset(nullptr);
      return false;
    }
  }

  /* perform the modification */
  auto rc = fn(entry->buffer.get());
  entry->version++;
  entry_lk.unlock();

  /* decrement the entries refcount and return */
  dropEntryReference(entry);
  return rc;
}

bool PageMap::loadPage(
    uint64_t disk_addr,
    uint64_t disk_size,
    PageBuffer* buffer) {
  assert(disk_addr > 0);
  assert(disk_size > 0);

  auto buf = malloc(disk_size);
  if (!buf) {
    return false;
  }

  auto rc = pread(fd_, buf, disk_size, disk_addr);
  if (rc > 0) {
    if (!buffer->decode((const char*) buf, disk_size)) {
      rc = -1;
    }
  }

  free(buf);
  return rc > 0;
}

void PageMap::flushPage(
    PageIDType page_id,
    uint64_t version,
    uint64_t disk_addr,
    uint64_t disk_size) {
  /* grab the main mutex and locate the page in our map */
  std::unique_lock<std::mutex> map_lk(mutex_);
  auto iter = map_.find(page_id);
  if (iter == map_.end()) {
    return;
  }

  /* increment the entries refcount and unlock the main mutex */
  auto entry = iter->second;
  entry->refcount.fetch_add(1);
  map_lk.unlock();

  /* grab the entries lock and drop the buffer it the version matches */
  std::unique_lock<std::mutex> entry_lk(entry->lock);
  if (entry->version == version) {
    entry->disk_addr = disk_addr;
    entry->disk_size = disk_size;
    entry->buffer.reset(nullptr);
  }

  entry_lk.unlock();

  /* decrement the entries refcount and return */
  dropEntryReference(entry);
}

void PageMap::deletePage(PageIDType page_id) {
  std::unique_lock<std::mutex> map_lk(mutex_);
  auto iter = map_.find(page_id);
  if (iter == map_.end()) {
    return;
  }

  auto entry = iter->second;
  map_.erase(iter);
  map_lk.unlock();

  dropEntryReference(entry);
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

