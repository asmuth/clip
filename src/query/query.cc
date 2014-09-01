/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <fnordmetric/query/query.h>
#include <fnordmetric/sql/backends/csv/csvbackend.h>
#include <fnordmetric/sql/parser.h>
#include <fnordmetric/sql/queryplanbuilder.h>
#include <fnordmetric/sql/resultlist.h>
#include <fnordmetric/sql/tablerepository.h>
#include <fnordmetric/sql_extensions/drawstatement.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace query {

Query::Query(
    const char* query_string,
    query::TableRepository* repo) :
    Query(query_string, strlen(query_string), repo) {}

Query::Query(
    const char* query_string,
    size_t query_string_len,
    TableRepository* repo) {
  query::Parser parser;

  if (query_string_len == 0) {
    RAISE(Parser::ParseError, "empty query");
  }

  if (!parser.parse(query_string, query_string_len)) {
    RAISE(
        Parser::ParseError,
        "can't figure out how to parse this, sorry :(");
  }

  draw_statements_.emplace_back();
  for (auto stmt : parser.getStatements()) {
    switch (stmt->getType()) {
      case query::ASTNode::T_DRAW:
        draw_statements_.back().emplace_back(new DrawStatement(stmt));
        break;
      case query::ASTNode::T_SELECT:
        addStatement(stmt, repo);
        break;
      case query::ASTNode::T_IMPORT:
        importTable(stmt, repo);
        break;
      default:
        RAISE(util::RuntimeException, "invalid statement");
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
  assert(index < results_.size()); // FIXPAUL
  return results_[index].get();
}

size_t Query::getNumResultLists() const {
  return results_.size();
}

size_t Query::getNumCharts() const {
  return charts_.size();
}

ui::Canvas* Query::getChart(size_t index) const {
  assert(index < charts_.size()); // FIXPAUL
  return charts_[index].get();
}
bool Query::addStatement(
    query::ASTNode* statement,
    query::TableRepository* repo) {
  QueryPlanBuilder query_plan_builder;

  auto query_plan = query_plan_builder.buildQueryPlan(statement, repo);
  if (query_plan == nullptr) {
    fprintf(stderr, "cant build statement");
    return false;
  }

  statements_.emplace_back(
      std::unique_ptr<QueryPlanNode>(query_plan),
      draw_statements_.back().empty() ?
          nullptr : draw_statements_.back().back().get());

  return true;
}

void Query::importTable(ASTNode* statement, TableRepository* repo) const {
  auto table_name = statement->getChildren()[0]->getToken()->getString();

  switch (statement->getToken()->getType()) {

    case Token::T_CSV:
      return repo->addTableRef(
          table_name,
          csv_backend::CSVBackend::openTable(statement));

    default:
      RAISE(util::RuntimeException, "can't import table, unknown table type");

  }
}

}
}
