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
