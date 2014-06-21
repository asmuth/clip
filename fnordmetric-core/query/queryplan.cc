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
}

kQueryScanMode QueryPlan::selectScanMode(const Query* query) const {
  return AGGREGATE_TIME_WINDOW; // FIXPAUL
}

}
}
