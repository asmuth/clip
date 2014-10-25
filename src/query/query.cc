/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <string.h>
#include <fnordmetric/query/query.h>
#include <fnordmetric/sql/parser/astnode.h>
#include <fnordmetric/sql/parser/parser.h>
#include <fnordmetric/sql/runtime/queryplanbuilder.h>
#include <fnordmetric/sql/runtime/resultlist.h>
#include <fnordmetric/sql/runtime/tablerepository.h>
#include <fnordmetric/sql/runtime/importstatement.h>
#include <fnordmetric/sql_extensions/drawstatement.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace query {

Query::Query(
    const char* query_string,
    size_t query_string_len,
    Runtime* runtime) :
    Query(std::string(query_string, query_string_len), runtime) {}

Query::Query(
    const std::string& query_string,
    Runtime* runtime) :
    Query(
        query_string,
        runtime,
        std::unique_ptr<TableRepository>(new TableRepository())) {}

Query::Query(
    const std::string& query_string,
    Runtime* runtime,
    std::unique_ptr<TableRepository> table_repo) :
    runtime_(runtime),
    table_repo_(std::move(table_repo)),
    query_plan_(table_repo_.get()) {
  auto statements = runtime->parser()->parseQuery(query_string);
  draw_statements_.emplace_back();

  for (const auto& stmt : statements) {
    switch (stmt->getType()) {
      case query::ASTNode::T_DRAW:
        draw_statements_.back().emplace_back(
            new DrawStatement(stmt.get(), runtime->compiler()));
        break;
      case query::ASTNode::T_SELECT:
        statements_.emplace_back(
            std::unique_ptr<QueryPlanNode>(
                runtime_->queryPlanBuilder()->buildQueryPlan(
                    stmt.get(), table_repo_.get())),
            draw_statements_.back().empty() ?
                nullptr : draw_statements_.back().back().get());
        break;
      case query::ASTNode::T_IMPORT:
        table_repo_->import(
            ImportStatement(stmt.get(), runtime_->compiler()),
            runtime_->backends());
        break;
      default:
        RAISE(kRuntimeError, "invalid statement");
    }
  }
}

void Query::execute() {
  for (const auto& stmt : statements_) {
    auto target = new ResultList();
    target->addHeader(stmt.first->getColumns());
    results_.emplace_back(target);

    if (stmt.second == nullptr) {
      stmt.first->setTarget(target);
      stmt.first->execute();
    } else {
      stmt.second->addSelectStatement(stmt.first.get(), target);
    }
  }

  for (const auto& draw_group : draw_statements_) {
    auto chart = new ui::Canvas();
    for (const auto& draw_stmt : draw_group) {
      draw_stmt->execute(chart);
    }
    charts_.emplace_back(chart);
  }
}

ResultList* Query::getResultList(size_t index) const {
  if (index >= results_.size()) {
    RAISE(kIndexError, "invalid index: %i", index);
  }

  return results_[index].get();
}

size_t Query::getNumResultLists() const {
  return results_.size();
}

size_t Query::getNumCharts() const {
  return charts_.size();
}

ui::Canvas* Query::getChart(size_t index) const {
  if (index >= charts_.size()) {
    RAISE(kIndexError, "invalid index: %i", index);
  }

  return charts_[index].get();
}

}
}
