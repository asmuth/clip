/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_GROUPBY_H
#define _FNORDMETRIC_SQL_GROUPBY_H
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <assert.h>
#include <sql/parser/astnode.h>
#include <sql/parser/token.h>
#include <sql/runtime/queryplannode.h>
#include <sql/runtime/symboltable.h>
#include <sql/runtime/compile.h>

namespace fnordmetric {
namespace query {

class GroupBy : public QueryPlanNode {
public:

  GroupBy(
      std::vector<std::string>&& columns,
      CompiledExpression* select_expr,
      CompiledExpression* group_expr,
      size_t scratchpad_size,
      QueryPlanNode* child) :
      columns_(std::move(columns)),
      select_expr_(select_expr),
      group_expr_(group_expr),
      scratchpad_size_(scratchpad_size),
      child_(child) {
    child->setTarget(this);
  }

  ~GroupBy() {
    for (auto& pair : groups_) {
      auto scratchpad = pair.second.scratchpad;

      if (scratchpad != nullptr) {
        free(scratchpad);
      }
    }
  }

  void execute() override {
    child_->execute();

    for (auto& pair : groups_) {
      auto& row = pair.second.row;
      emitRow(row.data(), row.size());
    }
  }

  bool nextRow(SValue* row, int row_len) override {
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
      group->scratchpad = malloc(scratchpad_size_);

      if (group->scratchpad == nullptr) {
        RAISE(kMallocError, "malloc() failed");
      }

      memset(group->scratchpad, 0, scratchpad_size_);
    } else {
      group = &group_iter->second;
    }

    /* execute select expresion and save results */
    executeExpression(
        select_expr_,
        group->scratchpad,
        row_len,
        row,
        &out_len,
        out);

    std::vector<SValue> row_vec;
    for (int i = 0; i < out_len; i++) {
      row_vec.push_back(out[i]);
    }

    /* update group */
    group->row = row_vec;

    return true;
  }

  size_t getNumCols() const override {
    return columns_.size();
  }

  const std::vector<std::string>& getColumns() const override {
    return columns_;
  }

protected:

  struct Group {
    std::vector<SValue> row;
    void* scratchpad;
  };

  std::vector<std::string> columns_;
  CompiledExpression* select_expr_;
  CompiledExpression* group_expr_;
  size_t scratchpad_size_;
  QueryPlanNode* child_;
  std::unordered_map<std::string, Group> groups_;
};

}
}
#endif
