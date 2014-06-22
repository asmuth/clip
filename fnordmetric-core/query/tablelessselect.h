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

namespace fnordmetric {
namespace query {

class TablelessSelect : public Executable {
public:

  static TablelessSelect* build(ASTNode* ast) {
    if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() != 1) {
      return nullptr;
    }

    auto select_list = ast->getChildren()[0];
    std::vector<std::string> columns;
    std::vector<std::unique_ptr<ASTNode>> expressions;

    for (auto col : select_list->getChildren()) {
      assert(*col == ASTNode::T_DERIVED_COLUMN); // FIXPAUL
      auto derived = col->getChildren();
      assert(derived.size() == 2); // FIXPAUL

      if (derived.size() == 2) {
        assert(*derived[1] == ASTNode::T_COLUMN_NAME); // FIXPAUL
        auto colname_token = derived[1]->getToken();
        assert(colname_token && *colname_token == Token::T_IDENTIFIER);
        columns.emplace_back(colname_token->getString());
        expressions.emplace_back(derived[0]->deepCopy());
      }
    }

    return new TablelessSelect(std::move(columns), std::move(expressions));
  }

  TablelessSelect(
      std::vector<std::string>&& columns,
      std::vector<std::unique_ptr<ASTNode>>&& expressions) :
      columns_(std::move(columns)),
      expressions_(std::move(expressions)) {}

  void execute() override {
    std::vector<std::unique_ptr<SValue>> row;

    for (int i = 0; i < columns_.size(); ++i) {
      row.emplace_back(expr(expressions_[i].get()));
    }

    emitRow(std::move(row));
  }

  size_t getNumCols() const override {
    return columns_.size();
  }

  void addRow(std::vector<std::unique_ptr<SValue>>&& row) override {
    assert(0);
  }

  const std::vector<std::string>& getColumns() const override {
    return columns_;
  }

protected:
  const std::vector<std::string> columns_;
  const std::vector<std::unique_ptr<ASTNode>> expressions_;
};

}
}
#endif
