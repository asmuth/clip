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

DocumentRef::DocumentRef(Collection* collection) :
    collection_(collection),
    dirty_(1) {}

void DocumentRef::revert() {
  // FIXPAUL free pages if alloced
}

bool DocumentRef::isDirty() {
  return dirty_ == 1;
}

}
