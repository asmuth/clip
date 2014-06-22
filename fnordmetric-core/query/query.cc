/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <string.h>
#include "query.h"

namespace fnordmetric {
namespace query {

Query* Query::parse(const char* query_string) {
  Parser parser;
  parser.parse(query_string, strlen(query_string));
  //parser.debugPrint();

  return new Query(parser.getStatements()[0]); // FIXPAUL
}

Query::Query(ASTNode* select_statement) : query_plan_(select_statement) {}

bool Query::execute() {
  return true;
}


}
}
