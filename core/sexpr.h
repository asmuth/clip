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
#include <functional>
#include <memory>
#include <string>

namespace fviz {

struct Expr;
using ExprStorage = std::unique_ptr<Expr, std::function<void (Expr*)>>;

ExprStorage expr_create_list();
ExprStorage expr_create_list(ExprStorage items);
ExprStorage expr_create_value(const std::string& str);
ExprStorage expr_create_value_literal(const std::string& str);

Expr* expr_next(Expr* expr);
const Expr* expr_next(const Expr* expr);
void expr_set_next(Expr* expr, ExprStorage next);
ExprStorage* expr_get_next_storage(Expr* expr);

bool expr_is_list(const Expr* expr);
bool expr_is_list(const Expr* expr, const std::string& head);
const Expr* expr_get_list(const Expr* expr);
ExprStorage* expr_get_list_storage(Expr* expr);

bool expr_is_value(const Expr* expr);
bool expr_is_value(const Expr* expr, const std::string& cmp);
bool expr_is_value_literal(const Expr* expr);
bool expr_is_value_literal(const Expr* expr, const std::string& cmp);
bool expr_is_value_quoted(const Expr* expr);
bool expr_is_value_quoted(const Expr* expr, const std::string& cmp);
const std::string& expr_get_value(const Expr* expr);

ExprStorage expr_clone(const Expr* e, int count=-1);

std::string expr_inspect(const Expr* expr);
std::string expr_inspect_list(const Expr* expr);

} // namespace fviz

