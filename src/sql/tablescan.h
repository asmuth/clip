/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_QUERY_TABLELESCAN_H
#define _FNORDMETRIC_QUERY_TABLELESCAN_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include <fnordmetric/sql/token.h>
#include <fnordmetric/sql/astnode.h>
#include <fnordmetric/sql/queryplannode.h>
#include <fnordmetric/sql/tablerepository.h>
#include <fnordmetric/sql/compile.h>
#include <fnordmetric/sql/execute.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace query {

class TableScan : public QueryPlanNode {
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
    if (!(from_list)) {
      RAISE(util::RuntimeException, "corrupt AST");
    }

    if (!(*from_list == ASTNode::T_FROM)) {
      return nullptr;
    }

    if (from_list->getChildren().size() != 1) {
      return nullptr;
    }

    /* get table reference */
    auto tbl_name = from_list->getChildren()[0];
    if (!(*tbl_name == ASTNode::T_TABLE_NAME)) {
      RAISE(util::RuntimeException, "corrupt AST");
    }

    auto tbl_name_token = tbl_name->getToken();
    if (!(tbl_name_token != nullptr)) {
      RAISE(util::RuntimeException, "corrupt AST");
    }

    auto tbl_ref = repo->getTableRef(tbl_name_token->getString());

    if (tbl_ref == nullptr) {
      RAISE(
          util::RuntimeException,
          "undefined table '%s'",
          tbl_name_token->getString().c_str());

      return nullptr;
    }

    /* get select list */
    if (!(*ast->getChildren()[0] == ASTNode::T_SELECT_LIST)) {
      RAISE(util::RuntimeException, "corrupt AST");
    }
    auto select_list = ast->getChildren()[0];

    /* resolve column references and compile ast */
    if (select_list == nullptr) {
      RAISE(util::RuntimeException, "corrupt AST");
    }

    if (!resolveColumns(select_list, tbl_ref)) {
      return nullptr;
    }

    /* compile select expression */
    size_t select_scratchpad_len = 0;
    auto select_expr = compileAST(select_list, &select_scratchpad_len);
    if (!(select_scratchpad_len == 0)) {
      RAISE(util::RuntimeException, "corrupt AST");
    }

    /* column names */
    std::vector<std::string> column_names;
    for (auto col : select_list->getChildren()) {
      if (col->getType() != ASTNode::T_DERIVED_COLUMN) {
        RAISE(util::RuntimeException, "corrupt AST");
      }

      auto derived = col->getChildren();

      // column with AS clause
      if (derived.size() == 2) {
        if (derived[1]->getType() != ASTNode::T_COLUMN_ALIAS) {
          RAISE(util::RuntimeException, "corrupt AST");
        }

        auto colname_token = derived[1]->getToken();

        if (!(colname_token && *colname_token == Token::T_IDENTIFIER)) {
          RAISE(util::RuntimeException, "corrupt AST");
        }

        column_names.emplace_back(colname_token->getString());
        continue;
      }

      // resolved column name
      if (derived.size() == 1 && *derived[0] == ASTNode::T_RESOLVED_COLUMN) {
        auto colname_token = derived[0]->getToken();

        if (!(colname_token && *colname_token == Token::T_IDENTIFIER)) {
          RAISE(util::RuntimeException, "corrupt AST");
        }

        column_names.emplace_back(colname_token->getString());
        continue;
      }

      // expression
      column_names.emplace_back("<expr>"); // FIXPAUL!!
    }

    /* get where expression */
    CompiledExpression* where_expr = nullptr;
    if (ast->getChildren().size() > 2) {
      ASTNode* where_clause = ast->getChildren()[2];
      if (!(where_clause)) {
        RAISE(util::RuntimeException, "corrupt AST");
      }

      if (!(*where_clause == ASTNode::T_WHERE)) {
        return nullptr;
      }

      if (where_clause->getChildren().size() != 1) {
        RAISE(util::RuntimeException, "corrupt AST");
      }

      auto e = where_clause->getChildren()[0];

      if (e == nullptr) {
        RAISE(util::RuntimeException, "corrupt AST");
      }

      if (!resolveColumns(e, tbl_ref)) {
        return nullptr;
      }

      size_t where_scratchpad_len = 0;
      where_expr = compileAST(e, &where_scratchpad_len);
      if (where_scratchpad_len != 0) {
        RAISE(
            util::RuntimeException,
            "where expressions can only contain pure functions\n");
      }
    }

    return new TableScan(
        tbl_ref,
        std::move(column_names),
        select_expr,
        where_expr);
  }

  TableScan(
      TableRef* tbl_ref,
      std::vector<std::string>&& columns,
      CompiledExpression* select_expr,
      CompiledExpression* where_expr):
      tbl_ref_(tbl_ref),
      columns_(std::move(columns)),
      select_expr_(select_expr),
      where_expr_(where_expr) {}

  void execute() override {
    tbl_ref_->executeScan(this);
    finish();
  }

  bool nextRow(SValue* row, int row_len) override {
    auto pred_bool = true;
    auto continue_bool = true;

    SValue out[128]; // FIXPAUL
    int out_len;

    if (where_expr_ != nullptr) {
      executeExpression(where_expr_, nullptr, row_len, row, &out_len, out);

      if (out_len != 1) {
        RAISE(
            util::RuntimeException,
            "WHERE predicate expression evaluation did not return a result");
      }

      pred_bool = out[0].getBool();
    }

    if (pred_bool) {
      executeExpression(select_expr_, nullptr, row_len, row, &out_len, out);
      continue_bool = emitRow(out, out_len);
    }

    return continue_bool;
  }

  size_t getNumCols() const override {
    return columns_.size();
  }

  const std::vector<std::string>& getColumns() const override {
    return columns_;
  }

protected:

  static bool resolveColumns(ASTNode* node, TableRef* tbl_ref) {
    if (node == nullptr) {
      RAISE(util::RuntimeException, "corrupt AST");
    }

    if (node->getType() == ASTNode::T_COLUMN_NAME) {
      auto token = node->getToken();
      if (!(token && *token == Token::T_IDENTIFIER)) {
        RAISE(util::RuntimeException, "corrupt AST");
      }

      auto col_index = tbl_ref->getColumnIndex(token->getString());
      if (col_index < 0) {
        RAISE(
            util::RuntimeException,
            "no such column: '%s'",
            token->getString().c_str());
        return false;
      }

      node->setType(ASTNode::T_RESOLVED_COLUMN);
      node->setID(col_index);
      return true;
    } else {
      for (const auto& child : node->getChildren()) {
        if (child == nullptr) {
          node->debugPrint(4);
          RAISE(util::RuntimeException, "corrupt AST");
        }

        if (!resolveColumns(child, tbl_ref)) {
          return false;
        }
      }

      return true;
    }
  }

  TableRef* const tbl_ref_;
  const std::vector<std::string> columns_;
  CompiledExpression* const select_expr_;
  CompiledExpression* const where_expr_;

};

}
}
#endif
