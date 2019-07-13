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
#include "element_factory.h"

#include <unordered_map>

namespace plotfx {

ReturnCode element_build(
    const Environment& env,
    const ElementMap& factory,
    const Expr* expr,
    ElementRef* elem) {
  if (!expr || !expr_is_value(expr)) {
    return ReturnCode::error("EARG", "expected an element name");
  }

  auto elem_name = expr_get_value(expr);
  auto elem_iter = factory.elements.find(elem_name);
  if (elem_iter == factory.elements.end()) {
    return ReturnCode::errorf("EARG", "no such element: $0", elem_name);
  }

  return elem_iter->second(env, expr, elem);
}

ReturnCode element_build_all(
    const Environment& env,
    const ElementMap& factory,
    const Expr* expr,
    std::vector<ElementRef>* elems) {
  for (; expr; expr = expr_next(expr)) {
    if (!expr_is_list(expr)) {
      return ReturnCode::error("EARG", "expected an element list");
    }

    ElementRef elem;
    if (auto rc = element_build(env, factory, expr_get_list(expr), &elem); !rc) {
      return rc;
    }

    elems->emplace_back(std::move(elem));
  }

  return OK;
}

void element_bind(
    ElementMap* factory,
    const std::string& name,
    ElementConfigureFn configure_fn) {
  factory->elements[name] = configure_fn;
}

} // namespace plotfx
