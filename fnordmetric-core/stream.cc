/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "stream.h"
#include "storagebackend.h"

namespace fnordmetric {

void IStream::appendRecord(const IRecord& record) const {
  assert(cursor_.get() != nullptr);
  //cursor_->appendRow(row);
}

}
