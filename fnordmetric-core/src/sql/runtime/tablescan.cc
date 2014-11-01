/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/runtime/tablescan.h>

namespace fnordmetric {
namespace query {

TableScan* TableScan::build(
    ASTNode* ast,
    TableRepository* repo,
    Compiler* compiler) {
  if (!(*ast == ASTNode::T_SELECT)) {
    return nullptr;
  }

  if (ast->getChildren().size() < 2) {
    return nullptr;
  }

  /* get FROM clause */
  ASTNode* from_list = ast->getChildren()[1];
  if (!(from_list)) {
    RAISE(kRuntimeError, "corrupt AST");
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
    RAISE(kRuntimeError, "corrupt AST");
  }

  auto tbl_name_token = tbl_name->getToken();
  if (!(tbl_name_token != nullptr)) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  auto tbl_ref = repo->getTableRef(tbl_name_token->getString());

  if (tbl_ref == nullptr) {
    RAISE(
        kRuntimeError,
        "undefined table '%s'",
        tbl_name_token->getString().c_str());

    return nullptr;
  }

  /* get select list */
  if (!(*ast->getChildren()[0] == ASTNode::T_SELECT_LIST)) {
    RAISE(kRuntimeError, "corrupt AST");
  }
  auto select_list = ast->getChildren()[0];

  /* resolve column references and compile ast */
  if (select_list == nullptr) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  if (!resolveColumns(select_list, nullptr, tbl_ref)) {
    return nullptr;
  }

  /* compile select expression */
  size_t select_scratchpad_len = 0;
  auto select_expr = compiler->compile(select_list, &select_scratchpad_len);
  if (!(select_scratchpad_len == 0)) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  /* column names */
  std::vector<std::string> column_names;
  for (auto col : select_list->getChildren()) {
    if (col->getType() != ASTNode::T_DERIVED_COLUMN) {
      RAISE(kRuntimeError, "corrupt AST");
    }

    auto derived = col->getChildren();

    // column with AS clause
    if (derived.size() == 2) {
      if (derived[1]->getType() != ASTNode::T_COLUMN_ALIAS) {
        RAISE(kRuntimeError, "corrupt AST");
      }

      auto colname_token = derived[1]->getToken();

      if (!(colname_token && *colname_token == Token::T_IDENTIFIER)) {
        RAISE(kRuntimeError, "corrupt AST");
      }

      column_names.emplace_back(colname_token->getString());
      continue;
    }

    // resolved column name
    if (derived.size() == 1 && *derived[0] == ASTNode::T_RESOLVED_COLUMN) {
      auto col_name = tbl_ref->getColumnName(derived[0]->getID());
      column_names.emplace_back(col_name);
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
      RAISE(kRuntimeError, "corrupt AST");
    }

    if (!(*where_clause == ASTNode::T_WHERE)) {
      return nullptr;
    }

    if (where_clause->getChildren().size() != 1) {
      RAISE(kRuntimeError, "corrupt AST");
    }

    auto e = where_clause->getChildren()[0];

    if (e == nullptr) {
      RAISE(kRuntimeError, "corrupt AST");
    }

    if (!resolveColumns(e, nullptr, tbl_ref)) {
      return nullptr;
    }

    size_t where_scratchpad_len = 0;
    where_expr = compiler->compile(e, &where_scratchpad_len);
    if (where_scratchpad_len != 0) {
      RAISE(
          kRuntimeError,
          "where expressions can only contain pure functions\n");
    }
  }

  return new TableScan(
      tbl_ref,
      std::move(column_names),
      select_expr,
      where_expr);
}

TableScan::TableScan(
    TableRef* tbl_ref,
    std::vector<std::string>&& columns,
    CompiledExpression* select_expr,
    CompiledExpression* where_expr):
    tbl_ref_(tbl_ref),
    columns_(std::move(columns)),
    select_expr_(select_expr),
    where_expr_(where_expr) {}

void TableScan::execute() {
  tbl_ref_->executeScan(this);
  finish();
}

bool TableScan::nextRow(SValue* row, int row_len) {
  auto pred_bool = true;
  auto continue_bool = true;

  SValue out[128]; // FIXPAUL
  int out_len;

  if (where_expr_ != nullptr) {
    executeExpression(where_expr_, nullptr, row_len, row, &out_len, out);

    if (out_len != 1) {
      RAISE(
          kRuntimeError,
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

size_t TableScan::getNumCols() const {
  return columns_.size();
}

const std::vector<std::string>& TableScan::getColumns() const {
  return columns_;
}

/* recursively walk the ast and resolve column references */
bool TableScan::resolveColumns(
    ASTNode* node,
    ASTNode* parent,
    TableRef* tbl_ref) {
  if (node == nullptr) {
    RAISE(kRuntimeError, "corrupt AST");
  }

  switch (node->getType()) {
    case ASTNode::T_ALL: {
      if (parent == nullptr || parent->getType() != ASTNode::T_SELECT_LIST) {
        RAISE(kRuntimeError, "invalid use of the * wildcard");
      }

      parent->removeChild(node);

      for (const auto& column : tbl_ref->columns()) {
        auto derived_col = new ASTNode(ASTNode::T_DERIVED_COLUMN);
        auto resolved_col = new ASTNode(ASTNode::T_RESOLVED_COLUMN);
        resolved_col->setID(tbl_ref->getColumnIndex(column));
        derived_col->appendChild(resolved_col);
        parent->appendChild(derived_col);
      }

      return true;
    }

    case ASTNode::T_COLUMN_NAME: {
      auto token = node->getToken();
      if (!(token && *token == Token::T_IDENTIFIER)) {
        RAISE(kRuntimeError, "corrupt AST");
      }

      auto col_index = tbl_ref->getColumnIndex(token->getString());
      if (col_index < 0) {
        RAISE(
            kRuntimeError,
            "no such column: '%s'",
            token->getString().c_str());
        return false;
      }

      node->setType(ASTNode::T_RESOLVED_COLUMN);
      node->setID(col_index);
      return true;
    }

    default: {
      for (const auto& child : node->getChildren()) {
        if (child == nullptr) {
          RAISE(kRuntimeError, "corrupt AST");
        }

        if (!resolveColumns(child, node, tbl_ref)) {
          return false;
        }
      }

      return true;
    }

  }
}

}
}
