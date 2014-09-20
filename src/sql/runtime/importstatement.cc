/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/parser/token.h>
#include <fnordmetric/sql/runtime/compile.h>
#include <fnordmetric/sql/runtime/execute.h>
#include <fnordmetric/sql/runtime/importstatement.h>
#include <fnordmetric/sql/svalue.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace query {

ImportStatement::ImportStatement(ASTNode* ast, Compiler* compiler) {
  if (ast->getChildren().size() < 2) {
    RAISE(util::RuntimeException, "corrupt ast: ASTNode::Import\n");
  }

  auto source_uri_sval = executeSimpleConstExpression(
      compiler,
      ast->getChildren().back());

  source_uri_ = source_uri_sval.toString();

  const auto& children = ast->getChildren();
  for (int i = 0; i < children.size() - 1; ++i) {
    tables_.emplace_back(children[i]->getToken()->getString());
  }
}

const std::string& ImportStatement::source_uri() const {
  return source_uri_;
}

const std::vector<std::string>& ImportStatement::tables() const {
  return tables_;
}

}
}
