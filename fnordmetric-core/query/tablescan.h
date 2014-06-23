/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_TABLELESCAN_H
#define _FNORDMETRIC_QUERY_TABLELESCAN_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "token.h"
#include "astnode.h"
#include "executable.h"
#include "tablerepository.h"

namespace fnordmetric {
namespace query {

class TableScan : public Executable {
public:

  static TableScan* build(ASTNode* ast, TableRepository* repo) {
    if (!(*ast == ASTNode::T_SELECT)) {
      return nullptr;
    }

    if (ast->getChildren().size() < 2) {
      return nullptr;
    }

    /* get FROM clause */
    ASTNode* from_list = ast->getChildren()[1];
    assert(from_list);
    if (!(*from_list == ASTNode::T_FROM)) {
      return nullptr;
    }

    if (from_list->getChildren().size() != 1) {
      return nullptr;
    }

    /* get table reference */
    auto tbl_name = from_list->getChildren()[0];
    assert(*tbl_name == ASTNode::T_TABLE_NAME);
    auto tbl_name_token = tbl_name->getToken();
    assert(tbl_name_token != nullptr);
    auto tbl_ref = repo->getTableRef(tbl_name_token->getString());

    if (tbl_ref == nullptr) {
      printf("error: undefined table '%s'\n",
          tbl_name_token->getString().c_str());
      return nullptr;
    }

    /* get select list */
    ASTNode* select_list = ast->getChildren()[0];
    assert(select_list);
    if (!(*select_list == ASTNode::T_SELECT_LIST)) {
      return nullptr;
    }

    std::vector<std::string> columns;
    std::vector<std::unique_ptr<ASTNode>> expressions;

    for (auto col : select_list->getChildren()) {
      assert(*col == ASTNode::T_DERIVED_COLUMN); // FIXPAUL
      auto derived = col->getChildren();
      auto e = derived[0]->deepCopy();
      expressions.emplace_back(e);

      if (!resolveColumns(e, tbl_ref)) {
        return nullptr;
      }

      if (derived.size() == 2) {
        assert(*derived[1] == ASTNode::T_COLUMN_ALIAS); // FIXPAUL
        auto colname_token = derived[1]->getToken();
        assert(colname_token && *colname_token == Token::T_IDENTIFIER);
        columns.emplace_back(colname_token->getString());
      } else {
        columns.emplace_back("unnamed"); // FIXPAULL
      }
    }

    /* get where expression */
    std::unique_ptr<ASTNode> where_expr(nullptr);
    if (ast->getChildren().size() > 2) {
      ASTNode* where_clause = ast->getChildren()[2];
      assert(where_clause);
      if (!(*where_clause == ASTNode::T_WHERE)) {
        return nullptr;
      }

      assert(where_clause->getChildren().size() == 1);
      auto e = where_clause->getChildren()[0]->deepCopy();
      where_expr.reset(e);

      if (!resolveColumns(e, tbl_ref)) {
        return nullptr;
      }
    }

    return new TableScan(
        tbl_ref,
        std::move(columns),
        std::move(expressions),
        std::move(where_expr));
  }

  TableScan(
      TableRef* tbl_ref,
      std::vector<std::string>&& columns,
      std::vector<std::unique_ptr<ASTNode>>&& expressions,
      std::unique_ptr<ASTNode>&& where_expr) :
      tbl_ref_(tbl_ref),
      columns_(std::move(columns)),
      expressions_(std::move(expressions)),
      where_expr_(std::move(where_expr)) {}

  void execute() override {
    tbl_ref_->executeScan(this);
  }

  bool nextRow(SValue* row, int row_len) override {
    /*
    setCurrentRow(&row);
    std::vector<SValue*> out_row;

    auto pred_bool = true;
    auto continue_bool = true;

    if (where_expr_.get() != nullptr) {
      auto pred = expr(where_expr_.get());
      pred_bool = pred->getBool();
      delete pred;
    }

    if (pred_bool) {
      for (int i = 0; i < columns_.size(); ++i) {
        out_row.emplace_back(expr(expressions_[i].get()));
      }

      continue_bool = emitRow(out_row);
    }

    return continue_bool;
    */
  }

  size_t getNumCols() const override {
    return columns_.size();
  }

  const std::vector<std::string>& getColumns() const override {
    return columns_;
  }

protected:

  static bool resolveColumns(ASTNode* node, TableRef* tbl_ref) {
    if (node->getType() == ASTNode::T_COLUMN_NAME) {
      auto token = node->getToken();
      assert(token && *token == Token::T_IDENTIFIER);

      auto col_index = tbl_ref->getColumnIndex(token->getString());
      if (col_index < 0) {
        printf("error: column not found: %s\n", token->getString().c_str());
        return false;
      }

      node->setType(ASTNode::T_RESOLVED_COLUMN);
      node->setID(col_index);
      return true;
    } else {
      for (const auto& child : node->getChildren()) {
        if (!resolveColumns(child, tbl_ref)) {
          return false;
        }
      }

      return true;
    }
  }

  TableRef* const tbl_ref_;
  const std::vector<std::string> columns_;
  const std::vector<std::unique_ptr<ASTNode>> expressions_;
  const std::unique_ptr<ASTNode> where_expr_;
};

}
}
#endif
