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
#include "executable.h"
#include "tablelessselect.h"

namespace fnordmetric {
namespace query {

Planner::Planner(ASTNode* select_statement) :
    executable_(plan(select_statement)) {}

Executable* Planner::plan(ASTNode* ast) {
  Executable* exec = nullptr;

  if ((exec = TablelessSelect::build(ast)) != nullptr) {
    return exec;
  }

  assert(0); // FIXPAUL cant build queryplan
}

std::unique_ptr<Executable> Planner::getExecutable() {
  assert(executable_ != nullptr);
  auto ptr = std::unique_ptr<Executable>(executable_);
  executable_ = nullptr;
  return ptr;
}

}
}
