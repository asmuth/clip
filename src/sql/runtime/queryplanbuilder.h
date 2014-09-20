/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
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
#include <fnordmetric/sql/parser/token.h>
#include <fnordmetric/sql/parser/astnode.h>
#include <fnordmetric/sql/runtime/queryplan.h>
#include <fnordmetric/sql/runtime/compile.h>

namespace fnordmetric {
namespace query {
class QueryPlanNode;
class TableRepository;
class Runtime;

/**
 * All QueryPlanBuilder imeplementations must be thread safe. Specifically they
 * must support calling the buildQueryPlan method concurrenctly from many
 * threads
 */
class QueryPlanBuilderInterface {
public:
  QueryPlanBuilderInterface(
      Compiler* compiler,
      const std::vector<std::unique_ptr<Backend>>& backends) :
      compiler_(compiler),
      backends_(backends) {}

  virtual ~QueryPlanBuilderInterface() {}

  virtual QueryPlanNode* buildQueryPlan(
      ASTNode* statement,
      TableRepository* repo) = 0;

protected:
  Compiler* compiler_;
  const std::vector<std::unique_ptr<Backend>>& backends_;
};

class QueryPlanBuilder : public QueryPlanBuilderInterface {
public:
  QueryPlanBuilder(
      Compiler* compiler,
      const std::vector<std::unique_ptr<Backend>>& backends);

  void buildQueryPlan(
      const std::vector<std::unique_ptr<ASTNode>>& statements,
      QueryPlan* query_plan);

  QueryPlanNode* buildQueryPlan(
      ASTNode* statement,
      TableRepository* repo) override;

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
  QueryPlanNode* buildOrderByClause(ASTNode* ast, TableRepository* repo);

  std::vector<std::unique_ptr<QueryPlanBuilderInterface>> extensions_;
};

}
}
#endif
