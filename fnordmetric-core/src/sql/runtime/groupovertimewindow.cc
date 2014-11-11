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
#include <fnordmetric/sql/runtime/compile.h>
#include <fnordmetric/sql/runtime/execute.h>

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
  scratchpad_.reset(malloc(scratchpad_size_));
  if (scratchpad_.get() == nullptr) {
    RAISE(kMallocError, "malloc() failed");
  }

  child->setTarget(this);
}

void GroupOverTimewindow::execute() {
  child_->execute();

  for (auto& group : groups_) {
    emitGroup(&group.second);
  }
}

bool GroupOverTimewindow::nextRow(SValue* row, int row_len) {
  SValue out[128]; // FIXPAUL
  int out_len;

  /* execute group expression */
  if (group_expr_ != nullptr) {
    executeExpression(group_expr_, nullptr, row_len, row, &out_len, out);
  }

  /* stringify expression results into group key */
  auto key_str = SValue::makeUniqueKey(out, out_len);

  /* get group */
  Group* group = nullptr;

  auto group_iter = groups_.find(key_str);
  if (group_iter == groups_.end()) {
    group = &groups_[key_str];
  } else {
    group = &group_iter->second;
  }

  /* execute time expression */
  executeExpression(time_expr_, nullptr, row_len, row, &out_len, out);
  if (out_len != 1) {
    RAISE(
        kRuntimeError,
        "time_expr in GROUP OVER TIMEWINDOW clause must return exactly one"
        " value, got %i",
        (int) out_len);
  }

  auto time = static_cast<uint64_t>(out[0].getTimestamp());

  /* add row to group */
  std::vector<SValue> row_vec;
  for (int i = 0; i < row_len; i++) {
    row_vec.push_back(row[i]);
  }

  group->rows.emplace_back(time, row_vec);

  return true;
}

void GroupOverTimewindow::emitGroup(Group* group) {
  auto& rows = group->rows;

  if (rows.size() == 0) {
    return;
  }

  /* sort rows */
  std::sort(
      rows.begin(),
      rows.end(),
      [] (
          const std::pair<uint64_t, std::vector<SValue>>& a,
          const std::pair<uint64_t, std::vector<SValue>>& b) {
            return a.first < b.first;
          });


  size_t window_start_idx = 0;
  size_t window_end_idx;
  uint64_t window_start_time = rows[0].first;

  do {
    /* search end of current window */
    auto window_end_time = window_start_time + window_ * 1000000;
    for (
        window_end_idx = window_start_idx;
        window_end_idx < rows.size() &&
            rows[window_end_idx].first < window_end_time;
        ++window_end_idx);

    printf("window %i, %i (%lu-%lu=%lu)\n",
        window_start_idx,
        window_end_idx,
        window_start_time,
        window_end_time,
        window_end_time - window_start_time);

    emitWindow(
        window_end_time,
        rows.begin() + window_start_idx,
        rows.begin() + window_end_idx);

    /* advance window */
    window_start_time += step_ * 1000000;
    while (window_start_idx <= window_end_idx &&
        rows[window_start_idx].first < window_start_time) {
      window_start_idx++;
    }

  } while (window_end_idx < rows.size());
}

void GroupOverTimewindow::emitWindow(
    uint64_t window_time,
    std::vector<std::pair<uint64_t, std::vector<SValue>>>::iterator
        window_begin,
    std::vector<std::pair<uint64_t, std::vector<SValue>>>::iterator
        window_end) {

  SValue out[128]; // FIXPAUL
  int out_len;

  memset(scratchpad_.get(), 0, scratchpad_size_);

  for (; window_begin != window_end; window_begin++) {
    executeExpression(
        select_expr_,
        scratchpad_.get(),
        window_begin->second.size(),
        window_begin->second.data(),
        &out_len,
        out);
  }

  emitRow(out, out_len);
}

size_t GroupOverTimewindow::getNumCols() const {
  return columns_.size();
}

const std::vector<std::string>& GroupOverTimewindow::getColumns() const {
  return columns_;
}

}
}

