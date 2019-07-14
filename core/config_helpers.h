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
