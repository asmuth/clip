/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_QUERY_PLANER_H
#define _FNORDMETRIC_QUERY_PLANER_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include "token.h"
#include "astnode.h"

namespace fnordmetric {
namespace query {
class QueryPlanNode;
class TableRepository;

class QueryPlanBuilder {
public:
  QueryPlanBuilder() {}
  virtual ~QueryPlanBuilder() {}

  virtual QueryPlanNode* buildQueryPlan(
      ASTNode* statement,
      TableRepository* repo) = 0;

};

class DefaultQueryPlanBuilder : public QueryPlanBuilder {
public:
  DefaultQueryPlanBuilder() {}

  QueryPlanNode* buildQueryPlan(
      ASTNode* statement,
      TableRepository* repo) override;

protected:

  QueryPlanNode* buildDrawStatement(ASTNode* ast);
  QueryPlanNode* buildSeriesStatement(ASTNode* ast, TableRepository* repo);
  QueryPlanNode* buildAxisStatement(ASTNode* ast, TableRepository* repo);

  /**
   * Returns true if the ast is a SELECT statement that has a GROUP BY clause,
   * otherwise false
   */
  bool hasGroupByClause(ASTNode* ast);

  /**
   * Returns true if the ast is a SELECT statement with a select list that
   * contains at least one aggregation expression, otherwise false.
   */
  bool hasAggregationInSelectList(ASTNode* ast);

  /**
   * Walks the ast recursively and returns true if at least one aggregation
   * expression was found, otherwise false.
   */
  bool hasAggregationExpression(ASTNode* ast);

  /**
   * Build a group by query plan node for a SELECT statement that has a GROUP
   * BY clause
   */
  QueryPlanNode* buildGroupBy(ASTNode* ast, TableRepository* repo);

  /**
   * Recursively walk the provided ast and search for column references. For
   * each found column reference, add the column reference to the provided
   * select list and replace the original column reference with an index into
   * the new select list.
   *
   * This is used to create child select lists for nested query plan nodes.
   */
  bool buildInternalSelectList(ASTNode* ast, ASTNode* select_list);

  QueryPlanNode* buildLimitClause(ASTNode* ast, TableRepository* repo);

};

}
}
#endif
