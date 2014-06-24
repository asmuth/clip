/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
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

}
}
