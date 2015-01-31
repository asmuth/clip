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
#include <assert.h>
#include "transaction.h"
#include "collection.h"
#include "documentref.h"
#include "snapshot.h"

namespace fnordmetric {

Transaction::Transaction(std::unique_ptr<Snapshot>) :
    running_(1) {}

Transaction::~Transaction() {
  if (running_) {
    rollback();
  }
}

DocumentRef* Transaction::createDocument(const DocumentKey& key ) {
  auto docref = new DocumentRef(key);
  dirty_documents_.push_back(docref);
  return docref;
}

bool Transaction::commit() {
  if (!running_) {
    return false;
  }

  //bool ret = collection_->commitTransaction(this);
  bool ret = true;

  for (const auto docref : dirty_documents_) {
    if (ret == false && docref->isDirty()) {
      docref->revert();
    }
    delete docref;
  }

  // if (ret == true && sync_mode == SYNC)
  // collection_->sync();
  // FIXPAUL MAX SYNC AGE!

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

