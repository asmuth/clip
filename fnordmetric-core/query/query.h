/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_QUERY_H
#define _FNORDMETRIC_QUERY_QUERY_H
#include <stdlib.h>
#include <string>
#include <vector>
#include "parser.h"
#include "planner.h"

namespace fnordmetric {
namespace query {

class Query {
public:
  static Query* parse(const char* query_string);
  explicit Query(ASTNode* select_statement);

  Query(const Query& copy) = delete;
  Query& operator=(const Query& copy) = delete;
  Query(Query&& move);

  bool execute();

protected:
  Planner query_plan_;
};

}
}
#endif
