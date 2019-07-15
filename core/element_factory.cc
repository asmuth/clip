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
#include "element_factory.h"
#include "environment.h"

#include <unordered_map>

namespace fviz {

ReturnCode element_build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem) {
  if (!expr || !expr_is_value(expr)) {
    return error(ERROR, "expected an element name");
  }

  auto elem_name = expr_get_value(expr);
  auto elem_iter = env.element_map.elements.find(elem_name);
  if (elem_iter == env.element_map.elements.end()) {
    return errorf(ERROR, "no such element: {}", elem_name);
  }

  return elem_iter->second(env, expr, elem);
}

ReturnCode element_build_all(
    const Environment& env,
    const Expr* expr,
    std::vector<ElementRef>* elems) {
  for (; expr; expr = expr_next(expr)) {
    if (!expr_is_list(expr)) {
      return error(ERROR, "expected an element list");
    }

    ElementRef elem;
    if (auto rc = element_build(env, expr_get_list(expr), &elem); !rc) {
      return rc;
    }

    elems->emplace_back(std::move(elem));
  }

  return OK;
}

ReturnCode element_build_list(
    const Environment& env,
    const Expr* expr,
    std::vector<ElementRef>* elems) {
  if (!expr_is_list(expr)) {
    return error(ERROR, "expected an element list");
  }

  return element_build_all(env, expr_get_list(expr), elems);
}

void element_bind(
    ElementMap* factory,
    const std::string& name,
    ElementConfigureFn configure_fn) {
  factory->elements[name] = configure_fn;
}

} // namespace fviz
