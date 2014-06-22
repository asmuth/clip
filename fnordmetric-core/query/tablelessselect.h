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

  TablelessSelect(ASTNode* select_list) {
    for (auto col : select_list->getChildren()) {
      assert(*col == ASTNode::T_DERIVED_COLUMN); // FIXPAUL
      auto derived = col->getChildren();
      assert(derived.size() == 2); // FIXPAUL

      if (derived.size() == 2) {
        assert(*derived[1] == ASTNode::T_COLUMN_NAME); // FIXPAUL
        auto colname_token = derived[1]->getToken();
        assert(colname_token && *colname_token == Token::T_IDENTIFIER);
        columns_.emplace_back(colname_token->getString());
        expressions_.emplace_back(derived[0]->deepCopy());
      }
    }
  }

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
  std::vector<std::string> columns_;
  std::vector<std::unique_ptr<ASTNode>> expressions_;
};

}
}
#endif
