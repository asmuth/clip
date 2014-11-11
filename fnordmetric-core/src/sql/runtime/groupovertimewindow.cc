/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/runtime/groupovertimewindow.h>

namespace fnordmetric {
namespace query {

GroupOverTimewindow::GroupOverTimewindow(
    std::vector<std::string>&& columns,
    CompiledExpression* time_expr,
    fnordmetric::IntegerType window,
    fnordmetric::IntegerType step,
    CompiledExpression* select_expr,
    CompiledExpression* group_expr,
    size_t scratchpad_size,
    QueryPlanNode* child) :
    time_expr_(time_expr),
    window_(window),
    step_(step),
    columns_(std::move(columns)),
    select_expr_(select_expr),
    group_expr_(group_expr),
    scratchpad_size_(scratchpad_size),
    child_(child) {
  printf("new group over clause with %i, %i\n", window, step);
  child->setTarget(this);
}

void GroupOverTimewindow::execute() {
  RAISE(kNotYetImplementedError, "NYI");
}

bool GroupOverTimewindow::nextRow(SValue* row, int row_len) {
  RAISE(kNotYetImplementedError, "NYI");
}

size_t GroupOverTimewindow::getNumCols() const {
  return columns_.size();
}

const std::vector<std::string>& GroupOverTimewindow::getColumns() const {
  return columns_;
}

}
}

