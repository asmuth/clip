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
#include <unordered_map>

#include "return_code.h"
#include "element.h"

namespace fviz {

struct ElementMap {
  std::unordered_map<std::string, ElementConfigureFn> elements;
};

ReturnCode element_build(
    const Environment& env,
    const Expr* expr,
    ElementRef* elem);

ReturnCode element_build_all(
    const Environment& env,
    const Expr* expr,
    std::vector<ElementRef>* elems);

ReturnCode element_build_list(
    const Environment& env,
    const Expr* expr,
    std::vector<ElementRef>* elems);

ReturnCode element_build_macro(
    const Environment& env,
    ExprStorage expr,
    ElementRef* elem);

void element_bind(
    ElementMap* factory,
    const std::string& name,
    ElementConfigureFn configure_fn);

} // namespace fviz
