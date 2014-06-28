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

bool Query::parse(
    const char* query_string,
    TableRepository* repo,
    std::vector<std::unique_ptr<Query>>* destination) {
  Parser parser;
  parser.parse(query_string, strlen(query_string));

  for (auto stmt : parser.getStatements()) {
    auto query_plan = QueryPlan::buildQueryPlan(stmt, repo);

    if (query_plan == nullptr) {
      return false;
    }

    destination->emplace_back(
        new Query(std::unique_ptr<Executable>(query_plan)));
  }

  return true;
}

Query::Query(std::unique_ptr<Executable>&& executable) :
    executable_(std::move(executable)),
    results_(executable_->getColumns()) {}

bool Query::execute() {
  printf("execute=%p\n", executable_.get());
  executable_->setTarget(&results_);
  executable_->execute();
  return true;
}

const ResultList& Query::getResults() {
  return results_;
}

}
}
