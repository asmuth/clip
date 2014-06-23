/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <vector>
#include "compile.h"
#include "svalue.h"

namespace fnordmetric {
namespace query {

bool executeExpression(
    CompiledExpression* expr,
    int row_len,
    const SValue* row,
    int* outc,
    SValue* outv) {
  int argc = 0;
  SValue argv[8];

  /* execute children */
  for (auto cur = expr->child; cur != nullptr; cur = cur->next) {
    assert(argc < sizeof(argv) / sizeof(SValue));

    int out_len = 0;
    if (!executeExpression(cur, row_len, row, &out_len, argv + argc)) {
      return false;
    }

    assert(out_len == 1);
    argc++;
  }

  /* execute expression */
  switch (expr->type) {

    case X_CALL: {
      expr->call(&expr->arg0, argc, argv, outv);
      *outc = 1;
      return true;
    }

    case X_LITERAL: {
      *outv = *static_cast<SValue*>(expr->arg0);
      *outc = 1;
      return true;
    }

    case X_MULTI: {
      *outc = argc;
      memcpy(outv, argv, sizeof(SValue) * argc);
      return true;
    }

    case X_INPUT: {
      uint64_t index = (uint64_t) expr->arg0;
      assert(index < row_len);
      *outv = row[index];
      *outc = 1;
      return true;
    }

  }

  assert(0);
}

}
}
