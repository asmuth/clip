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

namespace fnordmetric {
namespace query {

std::unique_ptr<Query> Query::parse(const char* query_string) {
  Parser parser;
  parser.parse(query_string, strlen(query_string));
  //parser.debugPrint();

  Planner plan(parser.getStatements()[0]); // FIXPAUL
  auto query = new Query(plan.getExecutable()); 
  return std::unique_ptr<Query>(query);
}

Query::Query(std::unique_ptr<Executable>&& executable) :
    executable_(std::move(executable)) {}

bool Query::execute() {
  printf("execute=%p\n", executable_.get());
  executable_->execute();
  return true;
}


}
}
