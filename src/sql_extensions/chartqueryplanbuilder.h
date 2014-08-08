/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_CHARTQUERYPLANBUILDER_H
#define _FNORDMETRIC_SQL_CHARTQUERYPLANBUILDER_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <assert.h>
#include <fnordmetric/sql/queryplanbuilder.h>

namespace fnordmetric {
namespace query {
class QueryPlanNode;
class TableRepository;
class ASTNode;

class ChartQueryPlanBuilder : public QueryPlanBuilderInterface {
public:
  ChartQueryPlanBuilder() {}

  QueryPlanNode* buildQueryPlan(
      ASTNode* ast,
      TableRepository* repo) const override;

protected:

  QueryPlanNode* buildDrawStatement(ASTNode* ast) const;
  QueryPlanNode* buildAxisStatement(ASTNode* ast, TableRepository* repo) const;

};

}
}
#endif
