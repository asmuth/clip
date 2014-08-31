/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql_extensions/drawstatement.h>
#include <fnordmetric/sql_extensions/barchartbuilder.h>

namespace fnordmetric {
namespace query {

DrawStatement::DrawStatement(
    kDrawStatementType type) :
    type_(type) {}


DrawStatement::DrawStatement(ASTNode* ast) {
  switch (ast->getToken()->getType()) {
    case Token::T_BAR:
      type_ = DrawStatement::T_BAR_CHART;
      break;
    case Token::T_LINE:
      type_ = DrawStatement::T_LINE_CHART;
      break;
    case Token::T_AREA:
      type_ = DrawStatement::T_AREA_CHART;
      break;
    default:
      RAISE(
          util::RuntimeException,
          "invalid chart type: %s",
          Token::getTypeName(ast->getToken()->getType()));
  }
}

void DrawStatement::execute(ui::Canvas* canvas) const {
  switch (type_) {
    case T_BAR_CHART:
      return executeWithType<BarChartBuilder>(canvas);
    //case T_LINE_CHART:
    //  return executeWithType<ui::LineChart>(canvas);
  }
}

}
}
