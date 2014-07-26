/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_TABLELESSSELECT_H
#define _FNORDMETRIC_QUERY_TABLELESSSELECT_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "token.h"
#include "astnode.h"
#include "executable.h"
#include "compile.h"
#include "execute.h"

namespace fnordmetric {
namespace query {

class TablelessSelect : public Executable {
public:

  static TablelessSelect* build(ASTNode* ast) {
    if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() != 1) {
      return nullptr;
    }

    auto select_list = ast->getChildren()[0];

    /* compile ast */
    size_t scratchpad_len = 0;
    auto expr = compileAST(select_list, &scratchpad_len);
    assert(scratchpad_len == 0);

    /* column names */
    std::vector<std::string> column_names;
    for (auto col : select_list->getChildren()) {
      assert(*col == ASTNode::T_DERIVED_COLUMN); // FIXPAUL
      auto derived = col->getChildren();

      if (derived.size() == 2) {
        assert(*derived[1] == ASTNode::T_COLUMN_ALIAS);
        auto colname_token = derived[1]->getToken();
        assert(colname_token && *colname_token == Token::T_IDENTIFIER);
        column_names.emplace_back(colname_token->getString());
      } else {
        column_names.emplace_back("unnamed");
      }
    }

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
    assert(0);
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
