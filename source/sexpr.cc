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
#include <assert.h>
#include "sexpr.h"

namespace plotfx {

const Expr& Expr::operator[](size_t i) const {
  assert(next);
  assert(i < next->size());
  return (*next)[i];
}

size_t Expr::size() const {
  if (next) {
    return next->size();
  } else {
    return 0;
  }
}

Expr::operator const std::string&() const {
  return value;
}


bool is_list(const Expr& e) {
  return e.kind == ExprKind::LIST;
}

bool is_value(const Expr& e) {
  switch (e.kind) {
    case ExprKind::VALUE_LITERAL:
    case ExprKind::VALUE:
      return true;
    default:
      return false;
  }
}

bool is_value(const Expr& e, const std::string& cmp) {
  switch (e.kind) {
    case ExprKind::VALUE_LITERAL:
    case ExprKind::VALUE:
      break;
    default:
      return false;
  }

  return e.value == cmp;
}

bool is_value_literal(const Expr& e) {
  return e.kind == ExprKind::VALUE_LITERAL;
}

bool is_value_literal(const Expr& e, const std::string& cmp) {
  return e.kind == ExprKind::VALUE_LITERAL && e.value == cmp;
}

bool is_value_quoted(const Expr& e) {
  return e.kind == ExprKind::VALUE;
}

bool is_value_quoted(const Expr& e, const std::string& cmp) {
  return e.kind == ExprKind::VALUE && e.value == cmp;
}

} // namespace plotfx

