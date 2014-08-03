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
#include <memory>
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/query/axisstatement.h>
#include <fnordmetric/query/backends/csv/csvbackend.h>
#include <fnordmetric/query/drawstatement.h>
#include <fnordmetric/query/executable.h>
#include <fnordmetric/query/parser.h>
#include <fnordmetric/query/query.h>
#include <fnordmetric/query/queryplan.h>
#include <fnordmetric/query/resultlist.h>
#include <fnordmetric/query/tablerepository.h>

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

  for (auto stmt : parser.getStatements()) {
    switch (stmt->getType()) {
      case query::ASTNode::T_SELECT:
      case query::ASTNode::T_AXIS:
      case query::ASTNode::T_DRAW:
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
  std::vector<std::vector<DrawStatement*>> draw_statements;
  DrawStatement* current_draw_statement = nullptr;
  draw_statements.emplace_back();

  for (const auto& stmt : statements_) {
    auto draw_stmt = dynamic_cast<query::DrawStatement*>(stmt.get());
    if (draw_stmt != nullptr) {
      current_draw_statement = draw_stmt;
      draw_statements.back().push_back(draw_stmt);
      continue;
    }

    auto axis_stmt = dynamic_cast<query::AxisStatement*>(stmt.get());
    if (axis_stmt != nullptr) {
      if (current_draw_statement == nullptr) {
        RAISE(util::RuntimeException, "DRAW AXIS without DRAW CHART");
      }

      current_draw_statement->addAxisStatement(axis_stmt);
      continue;
    }

    auto target = new ResultList();
    target->addHeader(stmt->getColumns());
    results_.emplace_back(target);

    if (current_draw_statement != nullptr) {
      current_draw_statement->addSelectStatement(stmt.get(), target);
    } else {
      stmt->setTarget(target);
      stmt->execute();
    }
  }

  for (const auto& draw_group : draw_statements) {
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

/*
bool Query::execute(ChartRenderTarget* target) {
  Drawable* drawable = nullptr;

  for (const auto& stmt : statements_) {

    if (drawable == nullptr) {
      continue;
    }

    auto series_stmt = dynamic_cast<query::SeriesStatement*>(stmt.get());
    if (series_stmt != nullptr) {
      series_stmt->execute();
      for (const auto& series : series_stmt->getSeries()) {
        drawable->addSeries(series);
      }
    }
  }

  if (drawable == nullptr) {
    // FIXPAUL add error no drawables defined
  } else {
    drawable->draw(target);
  }

  return true;
}

Drawable* Query::makeDrawable(query::DrawStatement* stmt) {
  switch (stmt->getType()) {
    case query::DrawStatement::T_BAR_CHART:
      return new BarChart();
    default:
      assert(0); // FIXPAUL
  }
}
*/

bool Query::addStatement(
    query::ASTNode* statement,
    query::TableRepository* repo) {
  auto query_plan = query::QueryPlan::buildQueryPlan(statement, repo);
  if (query_plan == nullptr) {
    fprintf(stderr, "cant build statement");
    return false;
  }

  statements_.emplace_back(query_plan);
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
