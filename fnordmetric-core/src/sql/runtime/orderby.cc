/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/runtime/orderby.h>

namespace fnordmetric {
namespace query {

OrderBy::OrderBy(
    std::vector<SortSpec> sort_specs,
    QueryPlanNode* child) :
    sort_specs_(sort_specs),
    child_(child) {
  child->setTarget(this);
}

void OrderBy::execute() {
  child_->execute();
}

bool OrderBy::nextRow(SValue* row, int row_len) {
  emitRow(row, row_len);
  return true;
}

size_t OrderBy::getNumCols() const {
  return child_->getNumCols();
}

const std::vector<std::string>& OrderBy::getColumns() const {
  return child_->getColumns();
}

} // namespace query
} // namespace fnordmetric
