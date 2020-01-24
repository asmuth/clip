/**
 * This file is part of the "clip" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "eval.h"
#include "graphics/export_image.h"
#include "graphics/export_svg.h"
#include "layout.h"
#include "sexpr_parser.h"
#include "commands.h"

namespace clip {

ReturnCode eval(
    Context* ctx,
    const Expr* expr) {
  // execute commands
  for (; expr; expr = expr_next(expr)) {
    if (!expr || !expr_is_list(expr)) {
      return error(ERROR, "expected a command list");
    }

    auto args = expr_get_list(expr);
    if (!args || !expr_is_value(args)) {
      return error(ERROR, "expected a command name");
    }

    auto arg0 = expr_get_value(args);

    const Command* cmd;
    if (auto cmd_iter = COMMANDS.find(arg0); cmd_iter != COMMANDS.end()) {
      cmd = &cmd_iter->second;
    } else {
      return {ERROR, fmt::format("Invalid command '{}'", arg0)};
    }

    if (auto rc = cmd->fn(ctx, args); !rc) {
      return rc;
    }
  }

  return OK;
}

ReturnCode eval(
    Context* ctx,
    const std::string& input) {
  ExprStorage expr;
  if (auto rc = expr_parse(input.data(), input.length(), &expr); !rc) {
    return rc;
  }

  return eval(ctx, expr.get());
}

} // namespace clip

