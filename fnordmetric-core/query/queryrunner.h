/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_QUERY_QUERYRUNNER_H
#define _FNORDMETRIC_QUERY_QUERYRUNNER_H
#include <stdlib.h>
#include <string>
#include <vector>
#include "query.h"
#include "queryplan.h"
#include "../database/database.h"
#include "../database/cursor.h"

namespace fnordmetric {
namespace query {

/**
 * The query runner is a singleton object that executes QueryPlans. The query
 * runner is threadsafe.
 */
class QueryRunner {
public:

  /**
   * Execute a query. This is threadsafe.
   */
  static void executeQuery(const QueryPlan& query_plan, Query* query);

protected:

  /**
   * Scan all streams in lockstep and aggregate data at the end of every time
   * window
   */
  static void scanWithTimeWindow(const QueryPlan& query_plan, Query* query);


};

}
}
#endif
