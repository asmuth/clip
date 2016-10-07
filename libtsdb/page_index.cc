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
#include "page_index.h"

namespace tsdb {

PageIndex::PageIndex(
    PageType type) :
    type_(type),
    entries_(nullptr),
    splitpoints_(nullptr),
    size_(0),
    disk_addr_(0),
    disk_size_(0) {}

bool PageIndex::alloc(size_t nentries) {
  if (nentries == 0 || size_ > 0) {
    return false;
  }

  size_t nsplits = nentries - 1;
  auto alloc = malloc(
      sizeof(PageIndexEntry) * nentries +
      sizeof(PageIndexSplitpoint) * nsplits);

  if (!alloc) {
    return false;
  }

  entries_ = (PageIndexEntry*) alloc;
  splitpoints_ = (PageIndexSplitpoint*) (entries_ + nentries);
  size_ = nentries;
  return true;
}

PageIndex::~PageIndex() {
  if (entries_) {
    free(entries_);
  }
}

PageIndexEntry* PageIndex::getEntries() {
  return entries_;
}

const PageIndexEntry* PageIndex::getEntries() const {
  return entries_;
}

PageIndexSplitpoint* PageIndex::getSplitpoints() {
  return splitpoints_;
}

const PageIndexSplitpoint* PageIndex::getSplitpoints() const {
  return splitpoints_;
}

size_t PageIndex::getSize() const {
  return size_;
}

PageType PageIndex::getType() const {
  return type_;
}

bool PageIndex::hasDiskSnapshot() const {
  return disk_addr_ > 0;
}

void PageIndex::getDiskSnapshot(uint64_t* addr, uint64_t* size) const {
  *addr = disk_addr_;
  *size = disk_size_;
}

void PageIndex::setDiskSnapshot(uint64_t addr, uint64_t size) {
  disk_addr_ = addr;
  disk_size_ = size;
}

} // namespace tsdb

