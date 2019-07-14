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
#include <functional>
#include "sexpr.h"
#include "utils/return_code.h"

namespace fviz {

using ExprVisitor = std::function<ReturnCode (const Expr*)>;

ReturnCode expr_walk_map(
    const Expr* expr,
    const std::unordered_map<std::string, ExprVisitor>& fns);

ReturnCode expr_calln(
    const Expr* expr,
    const std::initializer_list<ExprVisitor>& fns);

ExprVisitor expr_calln_fn(const std::initializer_list<ExprVisitor>& fns);

} // namespace fviz

