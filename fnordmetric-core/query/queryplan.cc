/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include "queryplan.h"

namespace fnordmetric {
namespace query {

QueryPlan::QueryPlan(
    Query* query,
    database::Database* database) :
    scan_mode_(selectScanMode(query)) {
  for (const auto& stream : query->getStreams()) {
    // FIXPAUL get schema yadda yadda
    auto stream_ref = database->openStream(stream);
    stream_cursors_.push_back(stream_ref->getCursor());

    StreamStartPosition start_pos = {.unix_millis = 0, .logical_offset = 0};
    stream_start_positions_.push_back(start_pos);
  }
}


kQueryScanMode QueryPlan::selectScanMode(const Query* query) const {
  return AGGREGATE_TIME_WINDOW; // FIXPAUL
}

}
}
