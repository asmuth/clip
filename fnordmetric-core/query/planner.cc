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
#include "limitclause.h"
//#include "groupby.h"

namespace fnordmetric {
namespace query {

Executable* planQuery(ASTNode* ast, TableRepository* repo) {
  Executable* exec = nullptr;

  /* internal nodes: multi table query (joins), order, aggregation, limit */
  if ((exec = LimitClause::build(ast, repo)) != nullptr) {
    return exec;
  }

  //if ((exec = GroupBy::build(ast, repo)) != nullptr) {
  //  return exec;
  //}

  /* leaf nodes: table scan, tableless select */
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
