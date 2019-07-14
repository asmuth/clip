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
#include <assert.h>
#include "sexpr.h"

using namespace std::placeholders;

namespace fviz {

enum class ExprType {
  LIST, VALUE_LITERAL, VALUE
};

struct Expr {
  ExprType type;
  std::string value;
  ExprStorage list;
  ExprStorage next;
};

void expr_destroy(Expr* expr) {
  delete expr;
}

ExprStorage expr_create_list() {
  auto e = ExprStorage(new Expr, bind(&expr_destroy, _1));
  e->type = ExprType::LIST;
  return e;
}

ExprStorage expr_create_value(const std::string& str) {
  auto e = ExprStorage(new Expr, bind(&expr_destroy, _1));
  e->type = ExprType::VALUE;
  e->value = str;
  return e;
}

ExprStorage expr_create_value_literal(const std::string& str) {
  auto e = ExprStorage(new Expr, bind(&expr_destroy, _1));
  e->type = ExprType::VALUE_LITERAL;
  e->value = str;
  return e;
}

const Expr* expr_next(const Expr* expr) {
  return expr->next.get();
}

ExprStorage* expr_get_next_storage(Expr* expr) {
  return &expr->next;
}

bool expr_is_list(const Expr* expr) {
  return expr->type == ExprType::LIST;
}

const Expr* expr_get_list(const Expr* expr) {
  return expr->list.get();
}

ExprStorage* expr_get_list_storage(Expr* expr) {
  return &expr->list;
}

bool expr_is_value(const Expr* expr) {
  return expr->type == ExprType::VALUE || expr->type == ExprType::VALUE_LITERAL;
}

bool expr_is_value(const Expr* expr, const std::string& cmp) {
  return expr_is_value(expr) && expr->value == cmp;
}

bool expr_is_value_literal(const Expr* expr) {
  return expr->type == ExprType::VALUE_LITERAL;
}

bool expr_is_value_literal(const Expr* expr, const std::string& cmp) {
  return expr->type == ExprType::VALUE_LITERAL && expr->value == cmp;
}

bool expr_is_value_quoted(const Expr* expr) {
  return expr->type == ExprType::VALUE;
}

bool expr_is_value_quoted(const Expr* expr, const std::string& cmp) {
  return expr->type == ExprType::VALUE && expr->value == cmp;
}

const std::string& expr_get_value(const Expr* expr) {
  return expr->value;
}

} // namespace fviz

