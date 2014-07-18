/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include "documentref.h"

namespace fnordmetric {

DocumentRef::DocumentRef(
    const DocumentKey& key) :
    key_(key),
    dirty_(1) {}

void DocumentRef::revert() {
  // FIXPAUL free pages if alloced
}

bool DocumentRef::isDirty() {
  return dirty_ == 1;
}

void DocumentRef::setDocumentKey(const DocumentKey& key) {
  key_ = key;
}

const DocumentKey& DocumentRef::getDocumentKey() const {
  return key_;
}

void DocumentRef::getScratchpad(char** data, size_t* size) const {
  static char scratch[] = "fnord";
  *data = scratch;
  *size = sizeof(scratch);
}

}
