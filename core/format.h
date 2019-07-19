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
#include "return_code.h"

namespace fviz {

using Formatter = std::function<std::string (size_t idx, const std::string&)>;

Formatter format_decimal_fixed(size_t precision);
Formatter format_decimal_scientific(size_t precision);
Formatter format_datetime(const std::string& fmt);
Formatter format_string();
Formatter format_custom(const std::vector<std::string>& values);

ReturnCode format_configure(
    const Expr* expr,
    Formatter* formatter);

} // namespace fviz

