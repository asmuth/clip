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

