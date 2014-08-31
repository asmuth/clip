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

namespace fnordmetric {
namespace ui {
class Canvas;
}
namespace query {

class DrawStatement {
public:
  DrawStatement(ASTNode* ast);

  void addSelectStatement(QueryPlanNode* select_stmt, ResultList* target) {
    select_stmts_.emplace_back(select_stmt);
    result_lists_.emplace_back(target);
  }

  void execute(ui::Canvas* canvas) const;

  // FIXPAUL: who owns the chart?
  template <typename ChartBuilderType>
  void executeWithType(ui::Canvas* canvas) const {
    ChartBuilderType chart_builder(canvas, this);

    for (int i = 0; i < select_stmts_.size(); ++i) {
      const auto& stmt = select_stmts_[i];
      chart_builder.executeStatement(stmt); //, result_lists_[i]);
    }

    chart_builder.getChart();

    //for (const auto& axis_stmt : axis_stmts_) {
    //  axis_stmt.execute(drawable);
    //}
  }

  ASTNode const* getProperty(Token::kTokenType key) const;

protected:
  std::vector<QueryPlanNode*> select_stmts_;
  std::vector<ResultList*> result_lists_;
  ASTNode* ast_;
};

}
}
#endif
