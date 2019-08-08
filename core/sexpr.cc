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
#include "sexpr.h"

#include <assert.h>
#include <functional>
#include <sstream>

using namespace std::placeholders;
using std::bind;

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

ExprStorage expr_create_list(ExprStorage items) {
  auto e = ExprStorage(new Expr, bind(&expr_destroy, _1));
  e->type = ExprType::LIST;
  e->list = std::move(items);
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

Expr* expr_next(Expr* expr) {
  return expr->next.get();
}

const Expr* expr_next(const Expr* expr) {
  return expr->next.get();
}

void expr_set_next(Expr* expr, ExprStorage next) {
  expr->next = std::move(next);
}

ExprStorage* expr_get_next_storage(Expr* expr) {
  return &expr->next;
}

bool expr_is_list(const Expr* expr) {
  return expr && expr->type == ExprType::LIST;
}

bool expr_is_list(const Expr* expr, const std::string& head) {
  return expr_is_list(expr) && expr_is_value(expr_get_list(expr), head);
}

const Expr* expr_get_list(const Expr* expr) {
  return expr->list.get();
}

ExprStorage* expr_get_list_storage(Expr* expr) {
  return &expr->list;
}

bool expr_is_value(const Expr* expr) {
  return expr && expr->type == ExprType::VALUE || expr->type == ExprType::VALUE_LITERAL;
}

bool expr_is_value(const Expr* expr, const std::string& cmp) {
  return expr && expr_is_value(expr) && expr->value == cmp;
}

bool expr_is_value_literal(const Expr* expr) {
  return expr && expr->type == ExprType::VALUE_LITERAL;
}

bool expr_is_value_literal(const Expr* expr, const std::string& cmp) {
  return expr && expr->type == ExprType::VALUE_LITERAL && expr->value == cmp;
}

bool expr_is_value_quoted(const Expr* expr) {
  return expr && expr->type == ExprType::VALUE;
}

bool expr_is_value_quoted(const Expr* expr, const std::string& cmp) {
  return expr && expr->type == ExprType::VALUE && expr->value == cmp;
}

const std::string& expr_get_value(const Expr* expr) {
  return expr->value;
}

ExprStorage expr_clone(const Expr* e, int count /* =-1 */) {
  ExprStorage copy;
  ExprStorage* c = &copy;

  for (; e && count != 0; --count) {
    *c = ExprStorage(new Expr, bind(&expr_destroy, _1));
    (*c)->type = e->type;
    (*c)->value = e->value;

    if (e->list) {
      (*c)->list = expr_clone(e->list.get());
    }

    c = expr_get_next_storage(&**c);
    e = e->next.get();
  }

  return copy;
}

std::string expr_inspect(const Expr* expr) {
  std::stringstream s;

  switch (expr->type) {
    case ExprType::VALUE:
    case ExprType::VALUE_LITERAL:
      s << expr->value;
      break;
    case ExprType::LIST:
      s << "(";
      s << expr_inspect_list(expr_get_list(expr));
      s << ")";
      break;
  }

  return s.str();
}

std::string expr_inspect_list(const Expr* expr) {
  std::stringstream s;

  for (; expr; expr = expr_next(expr)) {
    s << expr_inspect(expr);

    if (expr_next(expr)) {
      s << " ";
    }
  }

  return s.str();
}

} // namespace fviz

