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

void AOCollection::sync() {
  // COPY INDEX
  // APPLY INBOX TO INDEX
  // MYSYNC SYNC WHOLE FILE
  //writeRootPage(current_index_page_);
}

std::unique_ptr<Cursor> AOCollection::Snapshot::getCursor(
    const DocumentKey& key) {
}

//Document* AOCollection::Transaction::getDocument(
//    const fnordmetric::Cursor* cursor) {
//}

}
