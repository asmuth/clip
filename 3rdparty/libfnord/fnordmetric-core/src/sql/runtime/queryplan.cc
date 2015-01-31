/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <sql/runtime/queryplan.h>

namespace fnordmetric {
namespace query {

QueryPlan::QueryPlan(TableRepository* table_repo) : table_repo_(table_repo) {}

void QueryPlan::addQuery(std::unique_ptr<QueryPlanNode> query) {
  queries_.emplace_back(std::move(query));
}

const std::vector<std::unique_ptr<QueryPlanNode>>& QueryPlan::queries() {
  return queries_;
}

TableRepository* QueryPlan::tableRepository() {
  return table_repo_;
}

}
}

