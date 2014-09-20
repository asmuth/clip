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
#include <fnordmetric/sql/expressions/math.h>

namespace fnordmetric {
namespace query {

DefaultRuntime::DefaultRuntime() {
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
