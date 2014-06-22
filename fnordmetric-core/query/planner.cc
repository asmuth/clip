/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "planner.h"
#include "astnode.h"

namespace fnordmetric {
namespace query {

Planner::Planner(ASTNode* select_statement) :
    executable_(plan(select_statement)) {}

Executable* Planner::plan(ASTNode* ast) {
  return planTablelessSelect(ast);
}

Executable* Planner::planTablelessSelect(ASTNode* ast) {
  if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() != 1) {
    return nullptr;
  }

  return new TablelessSelect(ast->getChildren()[0]);
  printf("is tableless select!\n");
}

}
}
