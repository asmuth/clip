/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_SQL_QUERYPLANBUILDER_H
#define _FNORDMETRIC_SQL_QUERYPLANBUILDER_H
#include <memory>
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

/**
 * All QueryPlanBuilder imeplementations must be thread safe. Specifically they
 * must support calling the buildQueryPlan method concurrenctly from many
 * threads
 */
class QueryPlanBuilderInterface {
public:
  QueryPlanBuilderInterface() {}
  virtual ~QueryPlanBuilderInterface() {}

  virtual QueryPlanNode* buildQueryPlan(
      ASTNode* statement,
      TableRepository* repo) const = 0;

};

class QueryPlanBuilder : public QueryPlanBuilderInterface {
public:
  QueryPlanBuilder() {}

  QueryPlanNode* buildQueryPlan(
      ASTNode* statement,
      TableRepository* repo) const override;

  void extend(std::unique_ptr<QueryPlanBuilderInterface> other);

protected:

  /**
   * Returns true if the ast is a SELECT statement that has a GROUP BY clause,
   * otherwise false
   */
  bool hasGroupByClause(ASTNode* ast) const;

  /**
   * Returns true if the ast is a SELECT statement with a select list that
   * contains at least one aggregation expression, otherwise false.
   */
  bool hasAggregationInSelectList(ASTNode* ast) const;

  /**
   * Walks the ast recursively and returns true if at least one aggregation
   * expression was found, otherwise false.
   */
  bool hasAggregationExpression(ASTNode* ast) const;

  /**
   * Build a group by query plan node for a SELECT statement that has a GROUP
   * BY clause
   */
  QueryPlanNode* buildGroupBy(ASTNode* ast, TableRepository* repo) const;

  /**
   * Recursively walk the provided ast and search for column references. For
   * each found column reference, add the column reference to the provided
   * select list and replace the original column reference with an index into
   * the new select list.
   *
   * This is used to create child select lists for nested query plan nodes.
   */
  bool buildInternalSelectList(ASTNode* ast, ASTNode* select_list) const;

  QueryPlanNode* buildLimitClause(ASTNode* ast, TableRepository* repo) const;

  std::vector<std::unique_ptr<QueryPlanBuilderInterface>> extensions_;
};

}
}
#endif
