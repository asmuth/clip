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

//size_t Database::kMaxPageSizeSoft = 4194304;
//size_t Database::kMaxPageSizeHard = 33554432;

uint64_t AOCollection::kInitialIndexPageSize = 65535;

AOCollection::AOCollection(
    const Schema& schema,
    std::shared_ptr<PageManager> page_manager,
    std::unique_ptr<IDSequence> seq) :
    Collection(schema, page_manager),
    page_index_(std::shared_ptr<PageIndex>(new PageIndex(page_manager))),
    seq_(std::move(seq)),
    last_key_(0) {}

AOCollection::~AOCollection() {
  sync();
}

std::unique_ptr<fnordmetric::Collection::Snapshot> AOCollection::getSnapshot() {
  auto snapshot = new AOCollection::Snapshot();
  return std::unique_ptr<fnordmetric::Collection::Snapshot>(snapshot);
}

bool AOCollection::commitTransaction(const Transaction* transaction) {
  commit_mutex_.lock();
  auto index = page_index_->clone();

  for (auto docref : transaction->getDirtyDocuments()) {
    if (docref->isDirty()) {
      auto key = docref->getDocumentKey();
      if (!key.isIntKey()) {
        printf("error: unsupported key type\n"); // FIXPAUL
        goto rollback_tx;
      }

      if (key.getIntKey() == 0) {
        uint64_t next_key = seq_->getNext(last_key_);
        docref->setDocumentKey(DocumentKey(next_key));
        last_key_ = next_key;
      } else if (key.getIntKey() <= last_key_) {
        printf("error: keys must be monotonically increasing\n"); // FIXPAUL
        goto rollback_tx;
      }

      appendDocument(docref, index);
    }
  }

commit_tx:
  printf("commit!");
  page_index_mutex_.lock();
  page_index_ = std::shared_ptr<PageIndex>(index);
  page_index_mutex_.unlock();
  commit_mutex_.unlock();
  return true;

rollback_tx:
  // FIXPAUL if error free the index copy!
  commit_mutex_.unlock();
  return false;
}

void AOCollection::appendDocument(
    const DocumentRef* docref,
    PageIndex* index) {
  char* data;
  size_t size = 100;
  //docref->getBinary(&data, &size);
  uint64_t key = docref->getDocumentKey().getIntKey();
  assert(key > 0);

  size_t row_size = size + sizeof(DocHeader);
  assert(size > 0);

  auto index_ref = index->getIndexPageEntryForInsert(row_size);
  auto page_ref = page_manager_->getPage(
      PageManager::Page(index_ref->offset, index_ref->size));

  DocHeader* document = page_ref->structAt<DocHeader>(index_ref->used);
  document->key = key;
  document->size = size;
  //memcpy(row->data, data, size);
  //row->checksum = row->computeChecksum();

  if (index_ref->first_key == 0) {
    index_ref->first_key = key;
  }
  index_ref->used += row_size;
}

void AOCollection::sync() {
  sync_mutex_.lock();

  // capture free page list
  auto index = getPageIndex();
  // MYSYNC SYNC WHOLE FILE
  //writeRootPage(index_page);

  // release freed pages
  sync_mutex_.unlock();
}

std::shared_ptr<PageIndex> AOCollection::getPageIndex() const {
  page_index_mutex_.lock();
  auto index = page_index_;
  page_index_mutex_.unlock();
  return index;
}

std::unique_ptr<Cursor> AOCollection::Snapshot::getCursor(
    const DocumentKey& key) {
  assert(0);
}

//Document* AOCollection::Transaction::getDocument(
//    const fnordmetric::Cursor* cursor) {
//}

}
