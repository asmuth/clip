/**
 * This file is part of the "fviz" project
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

namespace fviz {

ReturnCode expr_walk_map(
    const Expr* expr,
    const std::unordered_map<std::string, ExprVisitor>& fns) {
  for (; expr; expr = expr_next(expr)) {
    if (!expr_is_value_literal(expr)) {
      return ReturnCode::error("EARG", "expected a literal");
    }

    auto param = expr_get_value(expr);
    const auto& fn = fns.find(param);
    if (fn == fns.end()) {
      return ReturnCode::errorf("EARG", "invalid paramter: '$0'", param);
    }

    if (expr = expr_next(expr); !expr) {
      return ReturnCode::errorf("EARG", "expected an argument for '$0'", param);
    }

    if (auto rc = fn->second(expr); !rc.isSuccess()) {
      return ReturnCode::errorf(
          "EPARSE",
          "error while parsing property '$0': $1",
          param,
          rc.getMessage());

      return rc;
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
  return bind(&expr_calln, _1, fns);
}

} // namespace fviz


