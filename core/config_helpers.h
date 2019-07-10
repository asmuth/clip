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
#pragma once
#include <unordered_map>
#include <optional>
#include <sexpr.h>
#include <graphics/measure.h>
#include <graphics/color.h>
#include "graphics/layout.h"
#include "utils/return_code.h"
#include "core/layout.h"

namespace plotfx {

using ParserFn = std::function<ReturnCode (const plist::Property&)>;

template <typename T>
using ParseToFn = std::function<ReturnCode (const plist::Property&, T* value)>;

using ParserDefinitions = std::unordered_map<std::string, ParserFn>;

ReturnCode parseAll(
    const plist::PropertyList& plist,
    const ParserDefinitions& pdefs);

template <typename T>
using EnumDefinitions = std::unordered_map<std::string, T>;

template<typename T>
ReturnCode parseEnum(
    const EnumDefinitions<T>& defs,
    const std::string& str,
    T* value);

ReturnCode parse_classlike(
    const plist::Property& prop,
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
    const plist::Property& prop,
    Measure* value);

ReturnCode configure_measure_opt(
    const plist::Property& prop,
    std::optional<Measure>* value);

ReturnCode configure_color(
    const plist::Property& prop,
    Color* value);

ParserFn configure_color_fn(Color* var);
ParserFn configure_color_opt(std::optional<Color>* var);

ReturnCode configure_float(
    const plist::Property& prop,
    double* value);

ReturnCode configure_string(
    const plist::Property& prop,
    std::string* value);

ReturnCode configure_direction(
    const plist::Property& prop,
    Direction* value);

ReturnCode configure_position(
    const plist::Property& prop,
    Position* value);

ReturnCode configure_float_opt(
    const plist::Property& prop,
    std::optional<double>* value);

ReturnCode configure_strings(
    const plist::Property& prop,
    std::vector<std::string>* data);

ReturnCode configure_measures(
    const plist::Property& prop,
    std::vector<Measure>* measures);


} // namespace plotfx

#include "config_helpers_impl.h"
