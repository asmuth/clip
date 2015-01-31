/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include "agent.h"
#include "query/query.h"

namespace fnordmetric {

std::unique_ptr<query::Query> Agent::executeQuery(
    const std::string& query_string) const {
  std::unique_ptr<query::Query> query(new query::Query(query_string, *this));

  return query;
}

std::shared_ptr<Stream> Agent::findStreamByName(
    const std::string& stream_name) const {
  //return std::shared_ptr<Stream>(nullptr);
}

}
