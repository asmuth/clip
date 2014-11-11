/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_GROUPOVERTIMEWINDOW_H
#define _FNORDMETRIC_SQL_GROUPOVERTIMEWINDOW_H
#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <assert.h>
#include <fnordmetric/sql/parser/astnode.h>
#include <fnordmetric/sql/parser/token.h>
#include <fnordmetric/sql/runtime/queryplannode.h>
#include <fnordmetric/sql/runtime/symboltable.h>
#include <fnordmetric/sql/runtime/compile.h>

namespace fnordmetric {
namespace query {

class GroupOverTimewindow : public QueryPlanNode {
public:

  GroupOverTimewindow(
      std::vector<std::string>&& columns,
      CompiledExpression* time_expr,
      fnordmetric::IntegerType window,
      fnordmetric::IntegerType step,
      size_t input_row_size,
      size_t input_row_time_index,
      CompiledExpression* select_expr,
      CompiledExpression* group_expr,
      size_t scratchpad_size,
      QueryPlanNode* child);

  void execute() override;
  bool nextRow(SValue* row, int row_len) override;

  size_t getNumCols() const override;
  const std::vector<std::string>& getColumns() const override;

protected:

  struct Group {
    std::vector<std::pair<uint64_t, std::vector<SValue>>> rows;
  };

  void emitGroup(Group* group);

  void emitWindow(
      uint64_t window_time,
      std::vector<std::pair<uint64_t, std::vector<SValue>>>::iterator
          window_begin,
      std::vector<std::pair<uint64_t, std::vector<SValue>>>::iterator
          window_end);

  std::vector<std::string> columns_;
  CompiledExpression* time_expr_;
  fnordmetric::IntegerType window_;
  fnordmetric::IntegerType step_;
  size_t input_row_size_;
  size_t input_row_time_index_;
  CompiledExpression* select_expr_;
  CompiledExpression* group_expr_;
  size_t scratchpad_size_;
  QueryPlanNode* child_;
  std::unique_ptr<void> scratchpad_;
  std::unordered_map<std::string, Group> groups_;
};

}
}
#endif
