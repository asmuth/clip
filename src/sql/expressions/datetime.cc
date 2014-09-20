/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <fnordmetric/sql/runtime/symboltable.h>
#include <fnordmetric/sql/svalue.h>

namespace fnordmetric {
namespace query {

static void checkArgs(const char* symbol, int argc, int argc_expected) {
  if (argc != argc_expected) {
    RAISE(
        util::RuntimeException,
        "wrong number of arguments for %s. expected: %i, got: %i",
        symbol,
        argc_expected,
        argc);
  }
}

static void fromTimestamp(
    void* scratchpad,
    int argc,
    SValue* argv,
    SValue* out) {
  checkArgs("FROM_TIMESTAMP", argc, 1);

  SValue tmp = *argv;
  tmp.tryTimeConversion();
  *out = SValue(tmp.getTimestamp());
}

SymbolTableEntry _from_timestamp_symbol("FROM_TIMESTAMP", &fromTimestamp);


}
}
