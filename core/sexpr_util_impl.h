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

template <typename T=void>
ExprStorage expr_build_next() {
  return ExprStorage(nullptr);
}

template <typename... T>
ExprStorage expr_build_next(ExprStorage head, T&&... tail) {
  auto n = expr_build_next(std::forward<T>(tail)...);

  auto e = head.get();
  while (e && expr_next(e)) {
    e = expr_next(e);
  }

  if (e) {
    expr_set_next(e, std::move(n));
    return head;
  } else {
    return n;
  }
}

template <typename... T>
ExprStorage expr_build_next(std::vector<ExprStorage> head, T&&... tail) {
  auto n = expr_build_next(std::forward<T>(tail)...);

  ExprStorage e;
  for (auto iter = head.rbegin(); iter != head.rend(); ++iter) {
    if (e) {
      expr_set_next(iter->get(), std::move(e));
    } else {
      expr_set_next(iter->get(), std::move(n));
    }

    e = std::move(*iter);
  }

  if (e) {
    return e;
  } else {
    return n;
  }
}

template <typename... T>
ExprStorage expr_build_next(std::string head, T&&... tail) {
  auto e = expr_create_value(head);
  auto n = expr_build_next(std::forward<T>(tail)...);
  expr_set_next(e.get(), std::move(n));
  return e;
}

template <typename... T>
ExprStorage expr_build(T&&... items) {
  return expr_create_list(expr_build_next(std::forward<T>(items)...));
}

} // namespace fviz

