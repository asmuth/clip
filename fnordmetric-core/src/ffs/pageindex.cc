/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdint.h>
#include "pageindex.h"

namespace fnordmetric {

size_t PageIndex::kTargetRowsPerPage = 1024;

PageIndex::PageIndex(std::shared_ptr<PageManager> page_manager) :
    page_manager_(page_manager),
    used_(0) {}

PageIndex::PageIndex(
    std::shared_ptr<PageManager> page_manager,
    const PageManager::Page& index_page,
    uint32_t used) :
    page_manager_(page_manager),
    index_page_(index_page),
    used_(used) {}

PageIndex::~PageIndex() {
  printf("page index freed!\n");
}

PageIndex::IndexPageEntry* PageIndex::getIndexPageEntryForInsert(size_t bytes) {
  if (used_ >= sizeof(IndexPageEntry)) {
    auto last_page = index_page_ref_->structAt<IndexPageEntry>(
        used_ - sizeof(IndexPageEntry));
    if (last_page->used + bytes < last_page->size) {
      return last_page;
    }
  }

  size_t estimated_page_size = bytes * kTargetRowsPerPage;
  if (used_ >= sizeof(IndexPageEntry)) {
    auto last_page = index_page_ref_->structAt<IndexPageEntry>(
        used_ - sizeof(IndexPageEntry));
    estimated_page_size += last_page->used;
    estimated_page_size /= 2;
  }

  if (estimated_page_size < bytes) {
    estimated_page_size = bytes;
  }

  auto page = page_manager_->allocPage(estimated_page_size);
  return appendPage(page);
}

size_t PageIndex::getNumPages() const {
  if (used_ == 0) {
    return 0;
  }

  return used_ / sizeof(IndexPageEntry);
}

PageIndex::IndexPageEntry* PageIndex::appendPage(
    const PageManager::Page& page) {
  PageIndex* index = nullptr;

  if (used_ == 0) {
    index_page_ = page_manager_->allocPage(kInitialIndexPageSize);
    index_page_ref_ = page_manager_->getPage(index_page_);
    used_ = 0;
  }

  if (used_ + sizeof(IndexPageEntry) >= index_page_.size) {
    auto new_page = page_manager_->allocPage(index_page_.size * 2);
    // FIXPAUL: just copying the data over doesnt handle deletions!
    auto dst = page_manager_->getPage(new_page);
    auto src = page_manager_->getPage(index_page_);
    memcpy(dst->structAt<void>(0), src->structAt<void>(0), used_);
    // FIXPAUL free old page!
    index_page_ = new_page;
    index_page_ref_ = page_manager_->getPage(index_page_);
  }

  auto new_entry = index_page_ref_->structAt<IndexPageEntry>(used_);
  new_entry->offset = page.offset;
  new_entry->size = page.size;
  new_entry->used = 0;
  new_entry->first_key = 0;
  used_ += sizeof(IndexPageEntry);
  return new_entry;
}

/*
IndexPageEntry* PageIndex::getIndexPageEntryAt(size_t n) const {
  auto offset = n * sizeof(IndexPageEntry);
  assert(offset < used_);
  return index_page_mapped_->structAt<IndexPage>(0);
}
*/

PageIndex* PageIndex::clone() {
  if (used_ == 0) {
    return new PageIndex(page_manager_);
  }

  auto copy = page_manager_->allocPage(index_page_.size);
  auto dst = page_manager_->getPage(copy);
  auto src = page_manager_->getPage(index_page_);
  memcpy(dst->structAt<void>(0), src->structAt<void>(0), used_);
  return new PageIndex(page_manager_, copy, used_);
}

}
