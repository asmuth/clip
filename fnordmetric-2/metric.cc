/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "metric.h"
#include "filebackend.h"

namespace fnordmetric {

void IMetric::recordSample(const std::vector<uint8_t>& row) const {
  assert(cursor_.get() != nullptr);
  cursor_->appendRow(row);
}

}
