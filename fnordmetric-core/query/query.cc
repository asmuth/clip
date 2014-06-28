/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <string.h>
#include <memory>
#include "query.h"
#include "parser.h"
#include "queryplan.h"
#include "executable.h"
#include "tablerepository.h"

namespace fnordmetric {
namespace query {

Query::Query(const char* query_string, TableRepository* repo) {
  Parser parser;
  parser.parse(query_string, strlen(query_string));

  for (auto stmt : parser.getStatements()) {
    switch (stmt->getType()) {
      case ASTNode::T_SELECT:
        addSelectStatement(stmt, repo);
        break;
      case ASTNode::T_SERIES:
        addSelectStatement(stmt, repo);
        break;
      default:
        assert(0 == 777);
    }
  }
}

bool Query::execute() {
  ResultList* result = results_.data();

  for (const auto& stmt : statements_) {
    stmt->setTarget(result);
    stmt->execute();
    result++;
  }

  return true;
}

const ResultList& Query::getResults(size_t statement_index) {
  return results_[statement_index];
}

bool Query::addSelectStatement(ASTNode* statement, TableRepository* repo) {
  auto query_plan = QueryPlan::buildQueryPlan(statement, repo);

  if (query_plan == nullptr) {
    return false;
  }

  statements_.emplace_back(query_plan);
  results_.emplace_back(query_plan->getColumns());
  return true;
}

}
}
