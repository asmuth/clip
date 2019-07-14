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
#include "data.h"
#include <assert.h>
#include <iostream>

namespace fviz {

size_t series_len(const Series& s) {
  return s.size();
}

bool series_is_numeric(const Series& s) {
  for (const auto& v : s) {
    if (!StringUtil::isNumber(v)) {
      size_t idx;
      try {
        std::stod(v, &idx);
      } catch (...) {
        return false;
      }

      if (idx != v.size()) {
        return false;
      }
    }
  }

  return true;
}

std::vector<double> series_to_float(const Series& s) {
  std::vector<double> sf;

  for (const auto& v : s) {
    sf.push_back(value_to_float(v));
  }

  return sf;
}

double value_to_float(const Value& v) {
  try {
    return std::stod(v);
  } catch (...) {
    return 0;
  }
}

Value value_from_float(double v) {
  return std::to_string(v);
}

std::vector<DataGroup> series_group(const Series& data) {
  std::vector<DataGroup> groups;
  std::unordered_map<Value, size_t> group_map;

  for (size_t idx = 0; idx < data.size(); ++idx) {
    size_t group_idx = group_map[data[idx]];
    if (!group_idx) {
      DataGroup g;
      g.key = data[idx];
      groups.emplace_back(g);
      group_idx = groups.size();
      group_map[data[idx]] = group_idx;
    }

    assert(group_idx > 0);
    groups[group_idx - 1].index.push_back(idx);
  }

  return groups;
}

std::vector<Color> series_to_colors(
    SeriesRef series,
    const ScaleConfig& domain_config,
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

} // namespace fviz

