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
#include "sexpr.h"
#include "graphics/brush.h"
#include "graphics/color.h"
#include "graphics/measure.h"

namespace fviz {

using ExprConv = std::function<ReturnCode (const Expr*)>;

template <typename T>
using ExprConvTo = std::function<ReturnCode (const Expr*, T* to)>;

ReturnCode expr_to_string(
    const Expr* expr,
    std::string* value);

ReturnCode expr_to_strings(
    const Expr* expr,
    std::vector<std::string>* values);

ReturnCode expr_to_stringset(
    const Expr* expr,
    std::set<std::string>* values);

ReturnCode expr_to_float64(
    const Expr* expr,
    double* value);

ReturnCode expr_to_float64_opt(
    const Expr* expr,
    std::optional<double>* value);

ReturnCode expr_to_measure(
    const Expr* expr,
    Measure* value);

ReturnCode expr_to_measure_opt(
    const Expr* expr,
    std::optional<Measure>* value);

ReturnCode expr_to_measures(
    const Expr* expr,
    std::vector<Measure>* measures);

ReturnCode expr_to_color(
    const Expr* expr,
    Color* value);

ReturnCode expr_to_color_opt(
    const Expr* expr,
    std::optional<Color>* var);

ReturnCode expr_to_stroke_style(
    const Expr* expr,
    StrokeStyle* style);

ReturnCode expr_to_copy(
    const Expr* expr,
    ExprStorage* copy);

template <typename T>
ReturnCode expr_to_enum(
    const Expr* expr,
    const std::unordered_map<std::string, T> value_defs,
    T* value);

template <typename T>
ExprConv expr_to_enum_fn(
    T* value,
    const std::unordered_map<std::string, T> value_defs);

template <typename T>
ReturnCode expr_tov(
    const Expr* expr,
    ExprConvTo<T> conv,
    std::vector<T>* values);

template <typename T>
ExprConv expr_tov_fn(
    ExprConvTo<T> conv,
    std::vector<T>* values);

} // namespace fviz

#include "sexpr_conv_impl.h"
