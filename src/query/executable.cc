/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include "executable.h"

namespace fnordmetric {
namespace query {

Executable::Executable() : target_(nullptr) {}

Executable::~Executable() {}

void Executable::setTarget(RowSink* target) {
  target_ = target;
}

bool Executable::emitRow(SValue* row, int row_len) {
  assert(target_ != nullptr);
  return target_->nextRow(row, row_len);
}

}
}
