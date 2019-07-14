/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "sexpr_util.h"

using namespace std::placeholders;

namespace plotfx {

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

} // namespace plotfx


