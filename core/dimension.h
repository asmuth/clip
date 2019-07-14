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
#include <unordered_map>
#include "core/scale.h"
#include "data_model.h"
#include "graphics/measure.h"

namespace fviz {

std::vector<Color> series_to_colors(
    SeriesRef series,
    const DomainConfig& domain_config,
    const ColorScheme& palette);

std::vector<Color> groups_to_colors(
    size_t count,
    const std::vector<DataGroup>& groups,
    const ColorScheme& palette);

std::vector<Measure> series_to_sizes(
    SeriesRef series,
    const DomainConfig& domain_config,
    const Measure& low,
    const Measure& high);


} // namespace fviz

