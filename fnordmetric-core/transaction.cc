/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "transaction.h"
#include "collection.h"
#include "documentref.h"

namespace fnordmetric {

Transaction::Transaction(Collection* collection) :
    collection_(collection),
    snapshot_(std::move(collection_->getSnapshot())),
    running_(1) {}

Transaction::~Transaction() {
  if (running_) {
    rollback();
  }
}

DocumentRef* Transaction::createDocument() {
  auto docref = new DocumentRef(collection_);
  dirty_documents_.push_back(docref);
  return docref;
}

bool Transaction::commit() {
  if (!running_) {
    return false;
  }

  bool ret = collection_->commitTransaction(this);

  for (const auto docref : dirty_documents_) {
    delete docref;
  }

  running_ = 0;
  return ret;
}

bool Transaction::rollback() {
  if (!running_) {
    return false;
  }

  for (const auto docref : dirty_documents_) {
    if (docref->isDirty()) {
      docref->revert();
    }
    delete docref;
  }

  running_ = 0;
  return true;
}

const std::vector<DocumentRef*>& Transaction::getDirtyDocuments() const {
  return dirty_documents_;
}

}

