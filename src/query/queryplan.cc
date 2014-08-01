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
#include "queryplan.h"
#include "astnode.h"
#include "axisstatement.h"
#include "executable.h"
#include "tablelessselect.h"
#include "tablescan.h"
#include "tablerepository.h"
#include "limitclause.h"
#include "groupby.h"
#include "symboltable.h"
#include "drawstatement.h"

namespace fnordmetric {
namespace query {

Executable* QueryPlan::buildQueryPlan(ASTNode* ast, TableRepository* repo) {
  Executable* exec = nullptr;

  /* axis statement */
  if (ast->getType() == ASTNode::T_AXIS) {
    return buildAxisStatement(ast, repo);
  }

  /* draw statement */
  if (ast->getType() == ASTNode::T_DRAW) {
    return buildDrawStatement(ast);
  }

  /* internal nodes: multi table query (joins), order, aggregation, limit */
  if ((exec = LimitClause::build(ast, repo)) != nullptr) {
    return exec;
  }

  if (hasGroupByClause(ast) || hasAggregationInSelectList(ast)) {
    return buildGroupBy(ast, repo);
  }

  /* leaf nodes: table scan, tableless select */
  if ((exec = TableScan::build(ast, repo)) != nullptr) {
    return exec;
  }

  if ((exec = TablelessSelect::build(ast)) != nullptr) {
    return exec;
  }

  // if verbose -> dump ast

  RAISE(
      util::RuntimeException,
      "can't figure out a query plan for this, sorry :(");
}

bool QueryPlan::hasGroupByClause(ASTNode* ast) {
  if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() < 2) {
    return false;
  }

  for (const auto& child : ast->getChildren()) {
    if (child->getType() == ASTNode::T_GROUP_BY) {
      return true;
    }
  }

  return false;
}

bool QueryPlan::hasAggregationInSelectList(ASTNode* ast) {
  if (!(*ast == ASTNode::T_SELECT) || ast->getChildren().size() < 2) {
    return false;
  }

  auto select_list = ast->getChildren()[0];
  assert(select_list->getType() == ASTNode::T_SELECT_LIST);

  return hasAggregationExpression(select_list);
}

bool QueryPlan::hasAggregationExpression(ASTNode* ast) {
  if (ast->getType() == ASTNode::T_METHOD_CALL) {
    assert(ast->getToken() != nullptr);
    auto symbol = lookupSymbol(ast->getToken()->getString());
    assert(symbol != nullptr); // FIXPAUL!!!!
    if (symbol->isAggregate()) {
      return true;
    }
  }

  for (const auto& child : ast->getChildren()) {
    if (hasAggregationExpression(child)) {
      return true;
    }
  }

  return false;
}

Executable* QueryPlan::buildDrawStatement(ASTNode* ast) {
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

Executable* QueryPlan::buildAxisStatement(
    ASTNode* ast,
    TableRepository* repo) {
  return new AxisStatement();
}

Executable* QueryPlan::buildGroupBy(ASTNode* ast, TableRepository* repo) {
  ASTNode group_exprs(ASTNode::T_GROUP_BY);
  assert(ast->getChildren()[0]->getType() == ASTNode::T_SELECT_LIST);

  /* copy own select list */
  auto select_list = ast->getChildren()[0]->deepCopy();

  /* generate select list for child */
  auto child_sl = new ASTNode(ASTNode::T_SELECT_LIST);
  buildInternalSelectList(select_list, child_sl);

  /* copy ast for child and swap out select lists*/
  auto child_ast = ast->deepCopy();
  child_ast->removeChild(0);
  child_ast->appendChild(child_sl, 0);

  /* search for a group by clause */
  for (const auto& child : ast->getChildren()) {
    if (child->getType() != ASTNode::T_GROUP_BY) {
      continue;
    }

    /* FIXPAUL resolve aliases in group list from select list, return error
       if alias contains aggregate func */

    /* copy all group expressions and add required field to child select list */
    for (const auto& group_expr : child->getChildren()) {
      auto e = group_expr->deepCopy();
      buildInternalSelectList(e, child_sl);
      group_exprs.appendChild(e);
    }

    /* remove group by clause from child ast */
    const auto& child_ast_children = child_ast->getChildren();
    for (int i = 0; i < child_ast_children.size(); ++i) {
      if (child_ast_children[i]->getType() == ASTNode::T_GROUP_BY) {
        child_ast->removeChild(i);
        break;
      }
    }
  }

  /* compile select list and group expressions */
  size_t select_scratchpad_len = 0;
  auto select_expr = compileAST(select_list, &select_scratchpad_len);

  size_t group_scratchpad_len = 0;
  auto group_expr = compileAST(&group_exprs, &group_scratchpad_len);
  assert(group_scratchpad_len == 0);
  //child_ast->debugPrint(2);
  //select_list->debugPrint(2);

  /* resolve output column names */
  std::vector<std::string> column_names;
  for (const auto& col : select_list->getChildren()) {
    column_names.push_back("unnamed");
  }

  return new GroupBy(
      std::move(column_names),
      select_expr,
      group_expr,
      select_scratchpad_len,
      buildQueryPlan(child_ast, repo));
}


bool QueryPlan::buildInternalSelectList(
    ASTNode* node,
    ASTNode* target_select_list) {
  /* search for column references recursively */
  if (node->getType() == ASTNode::T_COLUMN_NAME) {
    auto col_index = -1;

    /* check if this columns is already in the select list */
    const auto& candidates = target_select_list->getChildren();
    for (int i = 0; i < candidates.size(); ++i) {
      // FIXPAUL
    }

    /* otherwise add this column to the select list */
    if (col_index < 0) {
      auto derived = new ASTNode(ASTNode::T_DERIVED_COLUMN);
      derived->appendChild(node->deepCopy());
      target_select_list->appendChild(derived);
      col_index = target_select_list->getChildren().size() - 1;
    }

    node->setType(ASTNode::T_RESOLVED_COLUMN);
    node->setID(col_index);
    return true;
  } else {
    for (const auto& child : node->getChildren()) {
      if (!buildInternalSelectList(child, target_select_list)) {
        return false;
      }
    }

    return true;
  }
}


}
}
