/**
 * This file is part of the "clip" project
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
#include <functional>
#include "sexpr.h"
#include "return_code.h"

namespace clip {

using ExprVisitor = std::function<ReturnCode (const Expr*)>;

ReturnCode expr_walk_map(
    const Expr* expr,
    const std::unordered_map<std::string, ExprVisitor>& fns,
    bool strict = true);

ReturnCode expr_walk_map_with_defaults(
    const Expr* expr,
    const std::unordered_map<std::string, ExprStorage>& defaults,
    const std::unordered_map<std::string, ExprVisitor>& fns,
    bool strict = true);

ReturnCode expr_calln(
    const Expr* expr,
    const std::initializer_list<ExprVisitor>& fns);

ExprVisitor expr_calln_fn(const std::initializer_list<ExprVisitor>& fns);

ReturnCode expr_call_string(
    const Expr* expr,
    std::function<ReturnCode (const std::string&)>);

ExprVisitor expr_call_string_fn(std::function<ReturnCode (const std::string&)>);

std::vector<const Expr*> expr_collect(const Expr* expr);

ReturnCode expr_rewritev(
    const Expr* expr,
    const std::string& prefix,
    std::vector<ExprStorage>* destination);

template <typename... T>
ExprStorage expr_build(T&&... items);

std::vector<ExprStorage> expr_clonev(const std::vector<ExprStorage>& exprs);

ExprStorage expr_unwrap(ExprStorage expr);

} // namespace clip

#include "sexpr_util_impl.h"

