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
#include <assert.h>
#include <fnordmetric/sql/astnode.h>
#include <fnordmetric/sql_extensions/chartqueryplanbuilder.h>
#include <fnordmetric/sql_extensions/drawstatement.h>
#include <fnordmetric/sql_extensions/axisstatement.h>

namespace fnordmetric {
namespace query {

QueryPlanNode* ChartQueryPlanBuilder::buildQueryPlan(
      ASTNode* ast,
      TableRepository* repo) const {
  /* axis statement */
  if (ast->getType() == ASTNode::T_AXIS) {
    return buildAxisStatement(ast, repo);
  }

  /* draw statement */
  if (ast->getType() == ASTNode::T_DRAW) {
    return buildDrawStatement(ast);
  }

  return nullptr;
}

QueryPlanNode* ChartQueryPlanBuilder::buildDrawStatement(ASTNode* ast) const {
  DrawStatement::kDrawStatementType type;
  switch (ast->getToken()->getType()) {
    case Token::T_BAR:
      type = DrawStatement::T_BAR_CHART;
      break;
    case Token::T_LINE:
      type = DrawStatement::T_LINE_CHART;
      break;
    case Token::T_AREA:
      type = DrawStatement::T_AREA_CHART;
      break;
    default:
      RAISE(
          util::RuntimeException,
          "invalid chart type: %s",
          Token::getTypeName(ast->getToken()->getType()));
      return nullptr;
  }

  return new DrawStatement(type);
}

QueryPlanNode* ChartQueryPlanBuilder::buildAxisStatement(
    ASTNode* ast,
    TableRepository* repo) const {
  return new AxisStatement();
}

}}
