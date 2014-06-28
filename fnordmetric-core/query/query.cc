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
#include "parser.h"
#include "queryplan.h"
#include "executable.h"
#include "tablerepository.h"
#include "seriesstatement.h"
#include "drawstatement.h"
#include "../drawable.h"

namespace fnordmetric {
namespace query {

Query::Query(const char* query_string, TableRepository* repo) {
  Parser parser;

  if (!parser.parse(query_string, strlen(query_string))) {
    fprintf(stderr, "parser error\n");
  }

  for (auto stmt : parser.getStatements()) {
    switch (stmt->getType()) {
      case ASTNode::T_SELECT:
      case ASTNode::T_DRAW:
      case ASTNode::T_SERIES:
        addSelectStatement(stmt, repo);
        break;
      default:
        assert(0 == 777);
    }
  }
}

bool Query::execute() {
  ResultList* result = results_.data();

  for (const auto& stmt : statements_) {
    stmt->setTarget(result);
    stmt->execute();
    result++;
  }

  return true;
}

bool Query::execute(std::vector<std::unique_ptr<Drawable>>* dst) {
  Drawable* drawable;

  for (const auto& stmt : statements_) {
    auto draw_stmt = dynamic_cast<DrawStatement*>(stmt.get());
    if (draw_stmt != nullptr) {
      drawable = makeDrawable(draw_stmt);
      dst->emplace_back(drawable);
      continue;
    }

    if (drawable == nullptr) {
      continue;
    }

    auto series_stmt = dynamic_cast<SeriesStatement*>(stmt.get());
    if (series_stmt != nullptr) {
      series_stmt->execute();
      for (const auto& series : series_stmt->getSeries()) {
        drawable->addSeries(series);
      }
    }
  }

  return true;
}

Drawable* Query::makeDrawable(DrawStatement* stmt) {
  return new Drawable();
  //switch (stmt->getType()) {
  //  case DrawStatement::T_BAR_CHART:
  //}
}
const ResultList& Query::getResults(size_t statement_index) {
  return results_[statement_index];
}

bool Query::addSelectStatement(ASTNode* statement, TableRepository* repo) {
  auto query_plan = QueryPlan::buildQueryPlan(statement, repo);
  if (query_plan == nullptr) {
    fprintf(stderr, "error: cant build statement\n");
    return false;
  }

  statements_.emplace_back(query_plan);
  results_.emplace_back(query_plan->getColumns());
  return true;
}

}
}
