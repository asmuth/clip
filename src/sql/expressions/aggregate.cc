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
#include "../svalue.h"
#include "../symboltable.h"

namespace fnordmetric {
namespace query {

static void countExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  uint64_t* count = (uint64_t*) scratchpad;
  *out = SValue((int64_t) ++(*count));
}

static SymbolTableEntry __count_symbol(
    "count",
    &countExpr,
    sizeof(uint64_t));

/**
 * SUM() expression
 */
union sum_expr_scratchpad {
  uint64_t t_integer;
  double t_float;
};

static void sumExpr(void* scratchpad, int argc, SValue* argv, SValue* out) {
  SValue* val = argv;
  union sum_expr_scratchpad* data = (union sum_expr_scratchpad*) scratchpad;

  if (argc != 1) {
    RAISE(
        util::RuntimeException,
        "wrong number of arguments for sum(). expected: 1, got: %i\n",
        argc);
  }

  switch(val->getType()) {
    case SValue::T_INTEGER:
      data->t_integer += val->getInteger();
      *out = SValue((int64_t) data->t_integer);
      return;

    case SValue::T_FLOAT:
    default:
      data->t_float += val->getFloat();
      *out = SValue(data->t_float);
      return;
  }
}

static SymbolTableEntry __sum_symbol(
    "sum",
    &sumExpr,
    sizeof(union sum_expr_scratchpad));


}
}
