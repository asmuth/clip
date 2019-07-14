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
#include <optional>
#include <sexpr.h>
#include <graphics/measure.h>
#include <graphics/color.h>
#include "graphics/layout.h"
#include "utils/return_code.h"
#include "core/layout.h"

namespace fviz {

using ParserFn = std::function<ReturnCode (const Expr*)>;

template <typename T>
using ParseToFn = std::function<ReturnCode (const Expr*, T* value)>;

using ParserDefinitions = std::unordered_map<std::string, ParserFn>;

ReturnCode parseAll(
    const Expr* expr,
    const ParserDefinitions& pdefs);
/*
template <typename T>
using EnumDefinitions = std::unordered_map<std::string, T>;

template<typename T>
ReturnCode parseEnum(
    const EnumDefinitions<T>& defs,
    const std::string& str,
    T* value);

ReturnCode parse_classlike(
    const Expr* prop,
    const std::string& fn,
    std::vector<std::string>* args);

template <typename T>
ParserFn configure_opt(ParserFn parser);

template <typename T>
ParserFn configure_vec(ParseToFn<T> parser, std::vector<T>* values);

ParserFn configure_multiprop(const std::vector<ParserFn>& parsers);

ParserFn configure_alt(
    const ParserDefinitions& parsers);

ReturnCode configure_measure(
    const Expr* prop,
    Measure* value);

ReturnCode configure_measure_opt(
    const Expr* prop,
    std::optional<Measure>* value);

ReturnCode configure_color(
    const Expr* prop,
    Color* value);

ParserFn configure_color_fn(Color* var);
ParserFn configure_color_opt(std::optional<Color>* var);

ReturnCode configure_float(
    const Expr* prop,
    double* value);

ReturnCode configure_direction(
    const Expr* prop,
    Direction* value);

ReturnCode configure_position(
    const Expr* prop,
    Position* value);

ReturnCode configure_float_opt(
    const Expr* prop,
    std::optional<double>* value);

ReturnCode configure_strings(
    const Expr* prop,
    std::vector<std::string>* data);

ReturnCode configure_measures(
    const Expr* prop,
    std::vector<Measure>* measures);
*/

} // namespace fviz

#include "config_helpers_impl.h"
