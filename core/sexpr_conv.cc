/**
 * This file is part of the "fviz" project
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
#include "config_helpers.h"
#include "utils/fileutil.h"
#include "utils/csv.h"
#include "utils/algo.h"
#include <iostream>

using namespace std::placeholders;

namespace fviz {

ReturnCode expr_to_string(
    const Expr* expr,
    std::string* value) {
  if (!expr_is_value(expr)) {
    return ReturnCode::error("EARG", "expected value");
  }

  *value = expr_get_value(expr);
  return OK;
}

ReturnCode expr_to_measure(
    const Expr* expr,
    Measure* value) {
  if (!expr_is_value(expr)) {
    return ReturnCode::errorf(
        "EARG",
        "argument error; expected a value, got: $0",
        "..."); // FIXME
  }

  return parse_measure(expr_get_value(expr), value);
}

ReturnCode expr_to_measure_opt(
    const Expr* expr,
    std::optional<Measure>* value) {
  Measure v;
  if (auto rc = expr_to_measure(expr, &v); !rc) {
    return rc;
  }

  *value = v;
  return OK;
}

ReturnCode expr_to_color(
    const Expr* expr,
    Color* value) {
  if (!expr_is_value(expr)) {
    return ReturnCode::errorf(
        "EARG",
        "argument error; expected a value, got: $0",
        "..."); // FIXME
  }

  const auto expr_value = expr_get_value(expr);
  if (StringUtil::beginsWith(expr_value, "#")) {
    if (value->parse(expr_value)) {
      return OK;
    }
  }

  return ReturnCode::error("EARG", "invalid color");
}

ReturnCode expr_to_color_opt(
    const Expr* expr,
    std::optional<Color>* var) {
  Color c;
  if (auto rc = expr_to_color(expr, &c); !rc) {
    return rc;
  }

  *var = c;
  return OK;
}

} // namespace fviz

