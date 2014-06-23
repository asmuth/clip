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

static void countExpr(void** scratchpad, int argc, SValue** argv, SValue* out) {

}

static SymbolTableEntry __count_symbol(
    "count",
    &countExpr,
    true);

}
}
