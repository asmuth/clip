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
#include <assert.h>
#include <math.h>
#include <string.h>
#include "queryplannode.h"

namespace fnordmetric {
namespace query {

QueryPlanNode::QueryPlanNode() : target_(nullptr) {}

QueryPlanNode::~QueryPlanNode() {}

void QueryPlanNode::setTarget(RowSink* target) {
  target_ = target;
}

bool QueryPlanNode::emitRow(SValue* row, int row_len) {
  assert(target_ != nullptr);
  return target_->nextRow(row, row_len);
}

int QueryPlanNode::getColumnIndex(const std::string& column_name) const {
  const auto& columns = getColumns();

  for (int i = 0; i < columns.size(); ++i) {
    if (columns[i] == column_name) {
      return i;
    }
  }

  return -1;
}

void QueryPlanNode::finish() {
  if (target_ != nullptr) {
    target_->finish();
  }
}

}
}
