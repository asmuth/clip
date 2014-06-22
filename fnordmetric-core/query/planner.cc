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
#include "tablescan.h"
#include "tablerepository.h"

namespace fnordmetric {
namespace query {

Executable* planQuery(ASTNode* ast, TableRepository* repo) {
  Executable* exec = nullptr;

  // multi backend query
  // joins
  // table scan

  if ((exec = TableScan::build(ast, repo)) != nullptr) {
    return exec;
  }

  if ((exec = TablelessSelect::build(ast)) != nullptr) {
    return exec;
  }

  assert(0); // FIXPAUL cant build queryplan
}

}
}
