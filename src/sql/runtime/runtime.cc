/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/runtime/runtime.h>
#include <fnordmetric/sql/runtime/resultlist.h>
#include <fnordmetric/sql/backends/csv/csvbackend.h>

namespace fnordmetric {
namespace query {

Runtime::Runtime() :
    compiler_(&symbol_table_),
    query_plan_builder_(&compiler_, backends_) {}

QueryPlanBuilder* Runtime::queryPlanBuilder() {
  return &query_plan_builder_;
}

void Runtime::addBackend(std::unique_ptr<Backend> backend) {
  backends_.emplace_back(std::move(backend));
}

const std::vector<std::unique_ptr<Backend>>& Runtime::backends() {
  return backends_;
}

Compiler* Runtime::compiler() {
  return &compiler_;
}

Parser* Runtime::parser() {
  return &parser_;
}

}
}
