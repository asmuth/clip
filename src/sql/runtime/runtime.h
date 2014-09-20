/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_RUNTIME_H
#define _FNORDMETRIC_SQL_RUNTIME_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>

namespace fnordmetric {
namespace query {

/**
 * A runtime can only be used within a a single thread!
 */
class Runtime {
public:
  Runtime();

  std::unique_ptr<QueryPlan> buildQueryPlan

  void addQueryPlanBuilder(
      std::unique_ptr<QueryPlanBuilder> query_plan_builder);

protected:
  std::vector<std::unique_ptr<QueryPlanBuilder>> query_plan_builders_;
};

}
}
#endif
