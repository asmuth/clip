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
#include <string.h>
#include <vector>
#include <fnordmetric/sql/parser/astnode.h>
#include <fnordmetric/sql/runtime/compile.h>
#include <fnordmetric/sql/svalue.h>
#include <fnordmetric/util/runtimeexception.h>

namespace fnordmetric {
namespace query {

bool executeExpression(
    CompiledExpression* expr,
    void* scratchpad,
    int row_len,
    const SValue* row,
    int* outc,
    SValue* outv) {
  int argc = 0;
  SValue argv[8];

  /* execute children */
  for (auto cur = expr->child; cur != nullptr; cur = cur->next) {
    if (argc >= sizeof(argv) / sizeof(SValue)) {
      RAISE(util::RuntimeException, "too many arguments");
    }

    int out_len = 0;
    if (!executeExpression(
        cur,
        scratchpad,
        row_len,
        row,
        &out_len,
        argv + argc)) {
      return false;
    }

    if (out_len != 1) {
      RAISE(util::RuntimeException, "expression did not return");
    }

    argc++;
  }

  /* execute expression */
  switch (expr->type) {

    case X_CALL: {
      void* this_scratchpad = nullptr;
      if (scratchpad != nullptr) {
        this_scratchpad = ((char *) scratchpad) + ((size_t) (expr->arg0));
      }
      expr->call(this_scratchpad, argc, argv, outv);
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
      auto index = reinterpret_cast<uint64_t>(expr->arg0);

      if (index >= row_len) {
        RAISE(util::RuntimeException, "invalid row index %i", index);
      }

      *outv = row[index];
      *outc = 1;
      return true;
    }

  }
}

SValue executeSimpleConstExpression(Compiler* compiler, ASTNode* expr) {
  size_t scratchpad_len = 0;
  auto compiled = compiler->compile(expr, &scratchpad_len);

  if (scratchpad_len > 0) {
    RAISE(
        util::RuntimeException,
        "invalid const expression: const expressions must be pure functions");
  }

  SValue eval_result;
  int eval_result_len = 0;
  auto eval_retcode = executeExpression(
      compiled,
      nullptr,
      0,
      nullptr,
      &eval_result_len,
      &eval_result);

  if (!eval_retcode || eval_result_len != 1) {
    RAISE(
        util::RuntimeException,
        "invalid const expression: evaluation did not return a result");
  }

  return eval_result;
}

}
}
