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
#include <fnordmetric/util/runtimeexception.h>
#include <fnordmetric/sql/compile.h>
#include <fnordmetric/sql/execute.h>
#include <fnordmetric/sql/queryplannode.h>
#include <fnordmetric/sql/resultlist.h>
#include <fnordmetric/sql_extensions/axisstatement.h>

namespace fnordmetric {
namespace ui {
class Canvas;
}
namespace query {

class DrawStatement {
public:

  enum kDrawStatementType {
    T_BAR_CHART,
    T_LINE_CHART,
    T_AREA_CHART
  };

  DrawStatement(kDrawStatementType type);
  DrawStatement(ASTNode* ast);

  kDrawStatementType getType() const {
    return type_;
  }

  void addSelectStatement(QueryPlanNode* select_stmt, ResultList* target) {
    select_stmts_.emplace_back(select_stmt);
    result_lists_.emplace_back(target);
  }

  void addAxisStatement(const AxisStatement& axis_stmt) {
    axis_stmts_.emplace_back(axis_stmt);
  }

  void execute(ui::Canvas* canvas) const;

  template <typename ChartBuilderType>
  void executeWithType(ui::Canvas* canvas) const {
    ChartBuilderType chart_builder(canvas);

    for (int i = 0; i < select_stmts_.size(); ++i) {
      const auto& stmt = select_stmts_[i];
      chart_builder.executeStatement(stmt); //, result_lists_[i]);
    }

    auto drawable = chart_builder.getChart();

    for (const auto& axis_stmt : axis_stmts_) {
      axis_stmt.execute(drawable);
    }
  }

protected:
  std::vector<QueryPlanNode*> select_stmts_;
  std::vector<ResultList*> result_lists_;
  std::vector<AxisStatement> axis_stmts_;
  kDrawStatementType type_;
};

}
}
#endif
