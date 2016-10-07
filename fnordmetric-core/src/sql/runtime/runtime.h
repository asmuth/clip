/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_SQL_RUNTIME_H
#define _FNORDMETRIC_SQL_RUNTIME_H
#include <stdlib.h>
#include <string>
#include <vector>
#include <memory>
#include <metricd/sql/parser/astnode.h>
#include <metricd/sql/parser/parser.h>
#include <metricd/sql/runtime/compile.h>
#include <metricd/sql/runtime/queryplan.h>
#include <metricd/sql/runtime/queryplanbuilder.h>

namespace fnordmetric {
namespace query {
class ResultList;

/**
 * A runtime can only be used within a a single thread!
 */
class Runtime {
public:
  Runtime();

  void addBackend(std::unique_ptr<Backend> backend);

  Parser* parser();
  Compiler* compiler();
  const std::vector<std::unique_ptr<Backend>>& backends();
  QueryPlanBuilder* queryPlanBuilder();

protected:
  Parser parser_;
  SymbolTable symbol_table_;
  Compiler compiler_;
  std::vector<std::unique_ptr<Backend>> backends_;
  QueryPlanBuilder query_plan_builder_;
};

}
}
#endif
