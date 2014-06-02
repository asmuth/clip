/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_QUERYPLAN_H
#define _FNORDMETRIC_QUERY_QUERYPLAN_H
#include <stdlib.h>
#include <string>
#include <vector>
#include "query.h"
#include "../record.h"
#include "../database/database.h"
#include "../database/cursor.h"

namespace fnordmetric {
namespace query {

/**
 * Internal query scan modes:
 *
 * SCAN_LOCKSTEP
 *   Scan one or more streams in lockstep and return some data
 *
 * SCAN_SERIAL
 *   Scan one or more streams in serial order (scan one stream in full, then
 *   scan the next stream) and return some data
 *
 * AGGREGATE_TIME_WINDOW
 *   Scan one or more streams in lockstep and aggregate results per time window
 *
 * AGGREGATE_LOCKSTEP
 *   Scan one or more streams in lockstep and aggregate results after all
 *   streams were scanned.
 *
 * AGGREGATE_SERIAL
 *   Scan one or more streams in serial order (scan one stream in full, then
 *   scan the next stream) and aggregate results after all streams were scanned.
 *
 */
enum kQueryScanMode {
  SCAN_LOCKSTEP = 1,
  SCAN_TIME_WINDOW = 2,
  SCAN_SERIAL = 3
};

/**
 * A query plan describes what low level steps need to be taken to execute a
 * single query.
 *
 * Please not that since a QueryPlan will hold cursor objects to all streams
 * for the duration of its lifetime it should be create immideately before a
 * query is executed and destroyed as soon as the execution has finished.
 */
class QueryPlan {
  friend class QueryRunner;
public:

  /**
   * Describes the stream position to which a cursor should be set before
   * starting to scan
   */
  struct StreamStartPosition {
    uint64_t unix_millis;
    uint64_t logical_offset;
  };

  /**
   * Create a new query plan for a given query and database
   */
  explicit QueryPlan(Query* query, database::Database* database);
  QueryPlan(const QueryPlan& copy) = delete;
  QueryPlan& operator=(const QueryPlan& copy) = delete;
  QueryPlan(QueryPlan&& move);

protected:

  /**
   * The stream cursors for all streams to be scanned
   */
  std::vector<std::unique_ptr<database::Cursor>> stream_cursors_;

  /**
   * Record reader objects forr all streams to be scanned
   */
  std::vector<RecordReader> stream_readers_;

  /**
   * The start positions for all streams to be scanned
   */
  std::vector<StreamStartPosition> stream_start_positions_;

  /**
   * Select the optimal query scan mode for the given query
   */
  kQueryScanMode selectScanMode(const Query* query) const;

  /**
   * The query scan mode to be used for this execution plan
   */
  const kQueryScanMode scan_mode_;

  /**
   *
   */

};

}
}
#endif
