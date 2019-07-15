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
#include <string>
#include <vector>
#include "return_code.h"
#include "graphics/measure.h"
#include "scale.h"

namespace fviz {

using Value = std::string;
using Series = std::vector<Value>;
using SeriesRef = std::shared_ptr<const Series>;
using SeriesMap = std::unordered_map<std::string, SeriesRef>;

struct DataGroup {
  Value key;
  std::vector<size_t> index;
};

std::vector<DataGroup> series_group(const Series& data);

size_t series_len(const Series& s);

bool series_is_numeric(const Series& s);

std::vector<double> series_to_float(const Series& s);

double value_to_float(const Value&);
Value value_from_float(double);

std::vector<Color> series_to_colors(
    SeriesRef series,
    const ScaleConfig& domain_config,
    const ColorScheme& palette);

std::vector<Color> groups_to_colors(
    size_t count,
    const std::vector<DataGroup>& groups,
    const ColorScheme& palette);

std::vector<Measure> series_to_sizes(
    SeriesRef series,
    const ScaleConfig& domain_config,
    const Measure& low,
    const Measure& high);

ReturnCode data_load_strings(
    const Expr* expr,
    std::vector<std::string>* values);

ReturnCode data_load(
    const Expr* expr,
    std::vector<Measure>* values);

} // namespace fviz

