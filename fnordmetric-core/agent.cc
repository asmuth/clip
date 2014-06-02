/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include "agent.h"
#include "query/query.h"

namespace fnordmetric {

std::unique_ptr<query::Query> Agent::executeQuery(
    const std::string& query_string) const {
  std::unique_ptr<query::Query> query(new query::Query(query_string, *this));

  return std::unique_ptr<query::Query>(nullptr);
}

}
