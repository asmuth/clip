/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include "queryrunner.h"
#include "queryplan.h"

namespace fnordmetric {
namespace query {

void QueryRunner::executeQuery(const QueryPlan& query_plan, Query* query) {
  /* seek all cursors to their respective start positions */
  for (int i = 0; i < query_plan.stream_cursors_.size(); ++i) {
    auto start_pos = query_plan.stream_start_positions_[i];
    if (start_pos.logical_offset > 0) {
      query_plan.stream_cursors_[i]->seekToLogicalOffset(
          start_pos.logical_offset);
    } else if (start_pos.unix_millis > 0) {
      query_plan.stream_cursors_[i]->seekToTime(start_pos.unix_millis);
    } else {
      query_plan.stream_cursors_[i]->seekToFirst();
    }
  }

  /* scan all streams with the chosen scanning method */
  switch (query_plan.scan_mode_) {

    case SCAN_LOCKSTEP:
    case AGGREGATE_LOCKSTEP:
      // scanInLockStep(query_plan, query);
      break;

    case AGGREGATE_TIME_WINDOW:
      scanWithTimeWindow(query_plan, query);
      break;

    case SCAN_SERIAL:
    case AGGREGATE_SERIAL:
      // scanSerial(query_plan, query);
      break;

  };
}

void QueryRunner::scanWithTimeWindow(
    const QueryPlan& query_plan,
    Query* query) {
  uint64_t current_window;
  uint64_t window_size = 60;

}

}
}
