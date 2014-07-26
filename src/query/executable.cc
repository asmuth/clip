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

int Executable::getColumnIndex(const std::string& column_name) const {
  const auto& columns = getColumns();

  for (int i = 0; i < columns.size(); ++i) {
    if (columns[i] == column_name) {
      return i;
    }
  }

  return -1;
}

void Executable::finish() {
  if (target_ != nullptr) {
    target_->finish();
  }
}

}
}
