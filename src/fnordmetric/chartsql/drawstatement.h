/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_QUERY_DRAWSTATEMENT_H
#define _FNORDMETRIC_QUERY_DRAWSTATEMENT_H
#include <stdlib.h>
#include <fnordmetric/sql/runtime/compile.h>
#include <fnordmetric/sql/runtime/execute.h>
#include <fnordmetric/sql/runtime/queryplannode.h>
#include <fnordmetric/sql/runtime/resultlist.h>
#include <fnord-base/exception.h>
#include <fnord-chart/canvas.h>
#include <fnord-chart/drawable.h>

namespace fnordmetric {
namespace query {

class DrawStatement {
public:
  DrawStatement(ASTNode* ast, Compiler* compiler);

  void addSelectStatement(QueryPlanNode* select_stmt, ResultList* target) {
    select_stmts_.emplace_back(select_stmt);
    result_lists_.emplace_back(target);
  }

  void execute(fnord::chart::Canvas* canvas) const;

  ASTNode const* getProperty(Token::kTokenType key) const;

protected:

  template <typename ChartBuilderType>
  fnord::chart::Drawable* mkChart(fnord::chart::Canvas* canvas) const {
    ChartBuilderType chart_builder(canvas, this);

    for (int i = 0; i < select_stmts_.size(); ++i) {
      const auto& stmt = select_stmts_[i];
      chart_builder.executeStatement(stmt, result_lists_[i]);
    }

    return chart_builder.getChart();
  }

  void applyAxisDefinitions(fnord::chart::Drawable* chart) const;
  void applyAxisLabels(ASTNode* ast, fnord::chart::AxisDefinition* axis) const;
  void applyDomainDefinitions(fnord::chart::Drawable* chart) const;
  void applyGrid(fnord::chart::Drawable* chart) const;
  void applyLegend(fnord::chart::Drawable* chart) const;
  void applyTitle(fnord::chart::Drawable* chart) const;

  std::vector<QueryPlanNode*> select_stmts_;
  std::vector<ResultList*> result_lists_;
  ASTNode* ast_;
  Compiler* compiler_;
};

}
}
#endif
