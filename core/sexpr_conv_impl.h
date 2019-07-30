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
#pragma once

namespace fviz {

template <typename T>
ReturnCode expr_to_enum(
    const Expr* expr,
    const std::unordered_map<std::string, T> value_defs,
    T* value) {
  if (!expr || !expr_is_value(expr)) {
    return errorf(
        ERROR,
        "argument error; expected a list, got: {}",
        "..."); // FIXME
  }

  const auto& value_def = value_defs.find(expr_get_value(expr));
  if (value_def == value_defs.end()) {
    return errorf(
        ERROR,
        "invalid value '{}'",
        expr_get_value(expr));
  }

  *value = value_def->second;
  return OK;
}

template <typename T>
ExprConv expr_to_enum_fn(
    T* value,
    const std::unordered_map<std::string, T> value_defs) {
  return bind(
      &expr_to_enum<T>,
      std::placeholders::_1,
      value_defs,
      value);
}

template <typename T>
ReturnCode expr_tov(
    const Expr* expr,
    ExprConvTo<T> conv,
    std::vector<T>* values) {
  if (!expr || !expr_is_list(expr)) {
    return errorf(
        ERROR,
        "argument error; expected a list, got: {}",
        "..."); // FIXME
  }

  return expr_tov_flat<T>(expr_get_list(expr), conv, values);
}

template <typename T>
ExprConv expr_tov_fn(
    ExprConvTo<T> conv,
    std::vector<T>* values) {
  return bind(
      &expr_tov<T>,
      std::placeholders::_1,
      conv,
      values);
}

template <typename T>
ReturnCode expr_tov_flat(
    const Expr* expr,
    ExprConvTo<T> conv,
    std::vector<T>* values) {
  for (; expr; expr = expr_next(expr)) {
    T v;
    if (auto rc = conv(expr, &v); !rc) {
      return rc;
    }

    values->emplace_back(std::move(v));
  }

  return OK;
}

} // namespace fviz

