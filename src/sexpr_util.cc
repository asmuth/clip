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
#include "sexpr_util.h"

using namespace std::placeholders;

namespace clip {

void expr_each(
    const Expr* expr,
    std::function<void (const Expr* e)> fn) {
  for (; expr; expr = expr_next(expr)) {
    fn(expr);

    if (!expr_has_next(expr)) {
      break;
    }
  }
}

ReturnCode expr_walk_map(
    const Expr* expr,
    const std::unordered_map<std::string, ExprVisitor>& fns,
    bool strict /* = true */) {
  if (strict) {
    return expr_walk_map(expr, nullptr, fns);
  } else {
    ExprStorage unparsed;
    return expr_walk_map(expr, &unparsed, fns);
  }
}

ReturnCode expr_walk_map_wrapped(
    const Expr* expr,
    const std::unordered_map<std::string, ExprVisitor>& fns,
    bool strict /* = true */) {
  if (!expr || !expr_is_list(expr)) {
    return error(ERROR, "expected a list");
  }

  return expr_walk_map(expr_get_list(expr), fns, strict);
}

ReturnCode expr_walk_map(
    const Expr* expr,
    ExprStorage* unparsed,
    const std::unordered_map<std::string, ExprVisitor>& fns) {
  for (; expr; expr = expr_next(expr)) {
    if (!expr_is_list(expr)) {
      return errorf(ERROR, "expected a list, but got: {}", expr_inspect(expr));
    }

    auto args = expr_get_list(expr);
    if (!expr_is_value(args)) {
      return errorf(ERROR, "expected a literal, but got: {}", expr_inspect(args));
    }

    auto arg0 = expr_get_value(args);
    const auto& fn = fns.find(arg0);
    if (fn == fns.end()) {
      if (unparsed) {
        *unparsed = expr_clone(expr);
        unparsed = expr_get_next_storage(&**unparsed);
      } else {
        return errorf(ERROR, "invalid command: '{}'", arg0);
      }
    } else {
      if (auto rc = fn->second(expr_next(args)); !rc) {
        rc.trace.push_back(expr);
        return rc;
      }
    }

    if (!expr_has_next(expr)) {
      break;
    }
  }

  return OK;
}

ReturnCode expr_walk_tmap(
    const Expr* iter,
    const std::unordered_map<std::string, ExprVisitor>& fns,
    bool strict /* = true */) {
  for (; iter; iter = expr_next(iter)) {
    if (!expr_is_list(iter)) {
      return error(ERROR, "expected a list");
    }

    auto expr = expr_get_list(iter);
    if (!expr_is_value(expr)) {
      return error(ERROR, "expected a literal");
    }

    auto param = expr_get_value(expr);
    const auto& fn = fns.find(param);

    if (fn == fns.end()) {
      if (strict) {
        return errorf(ERROR, "invalid parameter: '{}'", param);
      }
    } else {
      if (auto rc = fn->second(expr_next(expr)); !rc) {
        rc.trace.push_back(expr);
        return rc;
      }
    }

    if (!expr_has_next(expr)) {
      break;
    }
  }

  return OK;
}

ReturnCode expr_calln(
    const Expr* expr,
    const std::initializer_list<ExprVisitor>& fns) {
  for (const auto& fn : fns) {
    if (auto rc = fn(expr); !rc) {
      return rc;
    }
  }

  return OK;
}

ExprVisitor expr_calln_fn(const std::initializer_list<ExprVisitor>& fns) {
  return std::bind(&expr_calln, _1, fns);
}

std::vector<const Expr*> expr_collect(const Expr* expr) {
  std::vector<const Expr*> exprs;

  while (expr) {
    exprs.push_back(expr);

    if (!expr_has_next(expr)) {
      break;
    }

    expr = expr_next(expr);
  }

  return exprs;
}

ReturnCode expr_call_string(
    const Expr* expr,
    std::function<ReturnCode (const std::string&)> fn) {
  if (!expr_is_value(expr)) {
    return {ERROR, "Expected a value"};
  }

  return fn(expr_get_value(expr));
}

ExprVisitor expr_call_string_fn(std::function<ReturnCode (const std::string&)> fn) {
  return std::bind(&expr_call_string, _1, fn);
}

ReturnCode expr_rewritev(
    const Expr* expr,
    const std::string& prefix,
    std::vector<ExprStorage>* destination) {
  destination->emplace_back(expr_create_value(prefix));
  destination->emplace_back(expr_clone(expr));
  return OK;
}

std::vector<ExprStorage> expr_clonev(const std::vector<ExprStorage>& exprs) {
  std::vector<ExprStorage> out;
  for (const auto& e : exprs) {
    out.emplace_back(expr_clone(e.get()));
  }

  return out;
}

ExprStorage expr_unwrap(ExprStorage expr) {
  if (expr && expr_is_list(expr.get())) {
    return std::move(*expr_get_list_storage(expr.get()));
  }

  return expr;
}

} // namespace clip


