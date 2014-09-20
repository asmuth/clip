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

Runtime::Runtime() {}

std::vector<std::unique_ptr<ASTNode>> Runtime::parseQuery(
    const std::string query) {
  if (query.size() == 0) {
    RAISE(Parser::ParseError, "empty query");
  }

  if (!parser_.parse(query.c_str(), query.size())) {
    RAISE(
        Parser::ParseError,
        "can't figure out how to parse this, sorry :(");
  }

  std::vector<std::unique_ptr<ASTNode>> stmts;
  for (const auto stmt : parser_.getStatements()) {
    stmts.emplace_back(stmt->deepCopy());
  }

  return stmts;
}

std::unique_ptr<QueryPlan> Runtime::buildQueryPlan(
    const std::vector<std::unique_ptr<ASTNode>>& statements) {
  std::unique_ptr<QueryPlan> query_plan(new QueryPlan());
  query_plan_builder_.buildQueryPlan(this, statements, query_plan.get());
  return query_plan;
}

QueryPlanBuilder* Runtime::queryPlanBuilder() {
  return &query_plan_builder_;
}

void Runtime::executeQuery(QueryPlanNode* query, ResultList* target) {
  target->addHeader(query->getColumns());
  query->setTarget(target);
  query->execute();
}

const std::vector<std::unique_ptr<Backend>>& Runtime::backends() {
  return backends_;
}

void Runtime::addBackend(std::unique_ptr<Backend> backend) {
  backends_.emplace_back(std::move(backend));
}

void Runtime::installBuiltinBackends() {
  addBackend(std::unique_ptr<Backend>(new csv_backend::CSVBackend()));
}

}
}
