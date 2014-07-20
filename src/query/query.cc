/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <string.h>
#include <memory>
#include "query.h"
#include "query/drawstatement.h"
#include "query/executable.h"
#include "query/parser.h"
#include "query/queryplan.h"
#include "query/resultlist.h"
#include "query/seriesstatement.h"
#include "query/tablerepository.h"
#include "query/resultlist.h"

namespace fnordmetric {
namespace query {

Query::Query(const char* query_string, query::TableRepository* repo) {
  query::Parser parser;

  if (!parser.parse(query_string, strlen(query_string))) {
    fprintf(stderr, "parser error\n");
  }

  for (auto stmt : parser.getStatements()) {
    switch (stmt->getType()) {
      case query::ASTNode::T_SELECT:
      case query::ASTNode::T_SERIES:
      case query::ASTNode::T_AXIS:
      case query::ASTNode::T_DRAW:
        addStatement(stmt, repo);
        break;
      default:
        assert(0 == 777);
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

    auto series_stmt = dynamic_cast<query::SeriesStatement*>(stmt.get());
    if (series_stmt != nullptr) {
      if (current_draw_statement == nullptr) {
        throw std::string("SERIES without BEGIN CHART");
      }

      current_draw_statement->addSeriesStatement(series_stmt);
      continue;
    }

    auto target = new ResultList();
    target->addHeader(stmt->getColumns());
    stmt->setTarget(target);
    stmt->execute();
    results_.emplace_back(target);
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
    fprintf(stderr, "error: cant build statement\n");
    return false;
  }

  statements_.emplace_back(query_plan);
  return true;
}

}
}
