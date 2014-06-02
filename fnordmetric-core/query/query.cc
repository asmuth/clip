/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include "query.h"

namespace fnordmetric {
namespace query {

Query::Query(const std::string& query_string) {
}

const std::vector<std::string>& Query::getStreams() {
  static std::vector<std::string> tmpstreams = {"mystream"};
  return tmpstreams; // FIXPAUL
}

}
}
