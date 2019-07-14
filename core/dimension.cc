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

