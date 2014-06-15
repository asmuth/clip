/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <algorithm>
#include <assert.h>
#include "aocollection.h"
#include "pagemanager.h"
#include "transaction.h"
#include "cursor.h"
#include "documentref.h"

namespace fnordmetric {

//size_t Database::kTargetRowsPerPage = 16384;
//size_t Database::kMaxPageSizeSoft = 4194304;
//size_t Database::kMaxPageSizeHard = 33554432;

uint64_t AOCollection::kInitialIndexPageSize = 65535;

AOCollection::AOCollection(
    const Schema& schema,
    std::shared_ptr<PageManager> page_manager) :
    Collection(schema, page_manager),
    page_index_(std::shared_ptr<PageIndex>(
        new PageIndex(PageManager::Page(0,0)))) {}

AOCollection::~AOCollection() {
  sync();
}

std::unique_ptr<fnordmetric::Collection::Snapshot> AOCollection::getSnapshot() {
  auto snapshot = new AOCollection::Snapshot();
  return std::unique_ptr<fnordmetric::Collection::Snapshot>(snapshot);
}

bool AOCollection::commitTransaction(const Transaction* transaction) {
  std::vector<PageAlloc> dirty_pages;

  commit_mutex_.lock();
  if (page_index_.getNumPages() > 0) {
    dirty_pages->push_back(*page_index->getLastPage());
  }

  for (auto docref : transaction->getDirtyDocuments()) {
    if (!docref->isDirty()) {
      delete docref;
      continue;
    }

    // FIXPAUL check that sequence number is increasing, assign seq num/time etc
    // FIXPAUL if error, revert and free all remaining docrefs and free the index
    // copy if existing!
    appendDocument(docref, &dirty_pages);
  }

  printf("commit!");
  page_index->commitChanges(dirty_pages);
  commit_mutex_.unlock();
}

void AOCollection::appendDocument(
    const DocumentRef* docref,
    std::vector<PageAlloc*>* dirty_pages) {
  char* data;
  size_t size;
  docref->getBinary(&data, &size);
  uint64_t key = 1235;

  size_t row_size = size + sizeof(DocHeader);
  assert(size > 0);

  if (dirty_pages->size() == 0) {
    auto estimated_page_size = estimatePageSize(row_size, row_size);
    auto page = page_manager_->allocPage(estimated_page_size);
    auto alloc = new PageAlloc(page, key);
    dirty_pages->push_back(alloc);
  }

  auto last_page = pages_.back();
  if (last_page->used_ + row_size >= last_page->page_.size) {
    assert(last_page->num_rows_.load() > 0);
    auto last_page_avg_size = last_page->used_ / last_page->num_rows_;
    auto estimated_page_size = estimatePageSize(last_page_avg_size, row_size);
    auto page = page_manager_->allocPage(estimated_page_size);
    auto alloc = new PageAlloc(page, time, logical_offset));
    pages_.push_back(alloc);
  }

  auto alloc = pages_.back();
  auto mmaped = page_manager_->getPage(alloc->page_);
  DocHeader* row = mmaped->structAt<RowHeader>(alloc->used_);
  //row->time = time;
  //row->size = size;
  //memcpy(row->data, data, size);
  //row->checksum = row->computeChecksum();

  //alloc->first_key = key;
  alloc->used_ += row_size;
  page->num_rows_++;

  return pos;
}
void AOCollection::sync() {
  sync_mutex_.lock();

  // capture free page list

  auto index_page = page_index_.snapshot();
  // MYSYNC SYNC WHOLE FILE
  //writeRootPage(index_page);

  // release freed pages
  sync_mutex_.unlock();
}

std::unique_ptr<Cursor> AOCollection::Snapshot::getCursor(
    const DocumentKey& key) {
}

//Document* AOCollection::Transaction::getDocument(
//    const fnordmetric::Cursor* cursor) {
//}

}
