/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_QUERY_TABLELESSSELECT_H
#define _FNORDMETRIC_QUERY_TABLELESSSELECT_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <sql/parser/astutil.h>
#include <sql/parser/astnode.h>
#include <sql/parser/token.h>
#include <sql/runtime/queryplannode.h>
#include <sql/runtime/compile.h>
#include <sql/runtime/execute.h>

namespace fnordmetric {
namespace query {

class TablelessSelect : public QueryPlanNode {
public:

  static TablelessSelect* build(ASTNode* ast, Compiler* compiler) {
    if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() != 1) {
      return nullptr;
    }

    auto select_list = ast->getChildren()[0];

    /* compile ast */
    size_t scratchpad_len = 0;
    auto expr = compiler->compile(select_list, &scratchpad_len);

    if (scratchpad_len > 0) {
      RAISE(
          kRuntimeError,
          "tableless SELECT can only contain pure functions");
    }

    /* column names */
    auto column_names = ASTUtil::columnNamesFromSelectList(select_list);
    return new TablelessSelect(std::move(column_names), expr);
  }

  TablelessSelect(
      std::vector<std::string>&& columns,
      CompiledExpression* expression) :
      columns_(std::move(columns)),
      expression_(expression) {}

  void execute() override {
    SValue row[128]; // FIXPAUL
    int row_len;

    executeExpression(expression_, nullptr, 0, nullptr, &row_len, row);
    emitRow(row, row_len);
    finish();
  }

  size_t getNumCols() const override {
    return columns_.size();
  }

  bool nextRow(SValue* row, int row_len) override {
    RAISE(kRuntimeError, "TablelessSelect#nextRow called");
  }

  const std::vector<std::string>& getColumns() const override {
    return columns_;
  }

protected:
  const std::vector<std::string> columns_;
  CompiledExpression* expression_;
};

}
}
#endif
