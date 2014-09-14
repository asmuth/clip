/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/sql/compile.h>
#include <fnordmetric/sql/execute.h>
#include <fnordmetric/sql/importstatement.h>
#include <fnordmetric/sql/svalue.h>
#include <fnordmetric/sql/token.h>

namespace fnordmetric {
namespace query {

ImportStatement::ImportStatement(ASTNode* ast) : ast_(ast) {
  if (ast->getChildren().size() < 2) {
    RAISE(util::RuntimeException, "corrupt ast: ASTNode::Import\n");
  }
}

std::string ImportStatement::source_uri() const {
  auto sval = executeSimpleConstExpression(ast_->getChildren().back());
  return sval.toString();
}

std::vector<std::string> ImportStatement::tables() const {
  std::vector<std::string> tables;

  const auto& children = ast_->getChildren();
  for (int i = 0; i < children.size() - 1; ++i) {
    tables.emplace_back(children[i]->getToken()->getString());
  }

  return tables;
}

}
}
