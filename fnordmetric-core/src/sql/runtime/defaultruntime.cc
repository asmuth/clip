/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql/runtime/defaultruntime.h>
#include <fnordmetric/sql/expressions/aggregate.h>
#include <fnordmetric/sql/expressions/boolean.h>
#include <fnordmetric/sql/expressions/datetime.h>
#include <fnordmetric/sql/expressions/math.h>

namespace fnordmetric {
namespace query {

DefaultRuntime::DefaultRuntime() {
  /* expressions/aggregate.h */
  symbol_table_.registerSymbol(
      "count",
      &expressions::countExpr,
      expressions::countExprScratchpadSize(),
      &expressions::countExprFree);

  symbol_table_.registerSymbol(
      "sum",
      &expressions::sumExpr,
      expressions::sumExprScratchpadSize(),
      &expressions::sumExprFree);

  symbol_table_.registerSymbol(
      "mean",
      &expressions::meanExpr,
      expressions::meanExprScratchpadSize(),
      &expressions::meanExprFree);

  symbol_table_.registerSymbol(
      "avg",
      &expressions::meanExpr,
      expressions::meanExprScratchpadSize(),
      &expressions::meanExprFree);

  symbol_table_.registerSymbol(
      "average",
      &expressions::meanExpr,
      expressions::meanExprScratchpadSize(),
      &expressions::meanExprFree);

  symbol_table_.registerSymbol(
      "min",
      &expressions::minExpr,
      expressions::minExprScratchpadSize(),
      &expressions::minExprFree);

  symbol_table_.registerSymbol(
      "max",
      &expressions::maxExpr,
      expressions::maxExprScratchpadSize(),
      &expressions::maxExprFree);

  /* expressions/boolean.h */
  symbol_table_.registerSymbol("eq", &expressions::eqExpr);
  symbol_table_.registerSymbol("neq", &expressions::neqExpr);
  symbol_table_.registerSymbol("and", &expressions::andExpr);
  symbol_table_.registerSymbol("or", &expressions::orExpr);
  symbol_table_.registerSymbol("neg", &expressions::negExpr);
  symbol_table_.registerSymbol("lt", &expressions::ltExpr);
  symbol_table_.registerSymbol("lte", &expressions::lteExpr);
  symbol_table_.registerSymbol("gt", &expressions::gtExpr);
  symbol_table_.registerSymbol("gte", &expressions::gteExpr);

  /* expressions/datetime.h */
  symbol_table_.registerSymbol("FROM_TIMESTAMP", &expressions::fromTimestamp);

  /* expressions/math.h */
  symbol_table_.registerSymbol("add", &expressions::addExpr);
  symbol_table_.registerSymbol("sub", &expressions::subExpr);
  symbol_table_.registerSymbol("mul", &expressions::mulExpr);
  symbol_table_.registerSymbol("div", &expressions::divExpr);
  symbol_table_.registerSymbol("mod", &expressions::modExpr);
  symbol_table_.registerSymbol("pow", &expressions::powExpr);
}

}
}
