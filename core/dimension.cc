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
#include "dimension.h"
#include <iostream>

namespace fviz {

std::vector<Color> series_to_colors(
    SeriesRef series,
    const DomainConfig& domain_config,
    const ColorScheme& palette) {
  if (!series) {
    return {};
  }

  auto domain = domain_config;
  //domain_fit(*series, &domain);

  std::vector<Color> colors;
  //for (const auto& v : *series) {
  //  auto value = domain_translate(domain, v) * domain_cardinality(domain);
  //  colors.emplace_back(palette.get(value));
  //}

  return colors;
}

std::vector<Color> groups_to_colors(
    size_t count,
    const std::vector<DataGroup>& groups,
    const ColorScheme& palette) {
  std::vector<Color> colors(count);
  for (size_t gi = 0; gi < groups.size(); ++gi) {
    for (auto i : groups[gi].index) {
      if (i < colors.size()) {
        colors[i] = palette.get(gi);
      }
    }
  }

  return colors;
}

/*
std::vector<Measure> series_to_sizes(
    SeriesRef series,
    const DomainConfig& domain_config,
    const Measure& low,
    const Measure& high) {
  if (!series) {
    return {};
  }

  auto domain = domain_config;
  domain_fit(*series, &domain);

  std::vector<Measure> sizes;
  for (const auto& v : *series) {
    auto value = domain_translate(domain, v);
    sizes.emplace_back(low.value + (high.value - low.value) * value);
  }

  return sizes;
}
*/

} // namespace fviz

