/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/parser/astnode.h>
#include <fnordmetric/sql/parser/astutil.h>
#include <fnordmetric/sql/parser/token.h>
#include <fnordmetric/sql/runtime/tablerepository.h>
#include <fnord/base/exception.h>

namespace fnordmetric {
namespace query {

std::vector<std::string> ASTUtil::columnNamesFromSelectList(
    ASTNode* select_list,
    TableRef* tbl_ref /* = nullptr */) {
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
      if (tbl_ref == nullptr) {
        column_names.emplace_back("col" + std::to_string(derived[0]->getID()));
      } else {
        auto col_name = tbl_ref->getColumnName(derived[0]->getID());
        column_names.emplace_back(col_name);
      }

      continue;
    }

    // expression
    column_names.emplace_back("<expr>"); // FIXPAUL!!
  }

  return column_names;
}

}
}
