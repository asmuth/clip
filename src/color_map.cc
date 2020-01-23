/**
 * This file is part of the "clip" project
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
#include "color_map.h"

namespace clip {

ColorMap color_map_gradient(std::vector<std::pair<double, Color>> gradient) {
  std::sort(
      gradient.begin(),
      gradient.end(),
      [] (const auto& a, const auto& b) {
        return a.first < b.first;
      });

  return [gradient] (const auto& value, Color* color) -> ReturnCode {
    if (gradient.empty()) {
      *color = Color{};
      return OK;
    }

    double value_num  = 0.0;
    try {
      value_num = std::clamp(std::stod(value), 0.0, 1.0);
    } catch (...) {
      return errorf(
          ERROR,
          "invalid data; can't map '{}' to a color",
          value);
    }

    auto step = std::lower_bound(
        gradient.begin(),
        gradient.end(),
        value_num,
        [] (const auto& a, auto b) {
          return a.first < b;
        });

    double limit0 = 0.0;
    auto limit1 = step->first;
    Color color0;
    auto color1 = step->second;

    if (step != gradient.end() && step != gradient.begin()) {
      limit0 = (step - 1)->first;
      color0 = (step - 1)->second;
    }

    auto weight1 = (value_num - limit0) / (limit1 - limit0);
    auto weight0 = 1.0 - weight1;

    if (value_num >= limit1) {
      weight1 = 1.0;
      weight0 = 0.0;
    }

    *color = Color::fromRGBA(
        color0[0] * weight0 + color1[0] * weight1,
        color0[1] * weight0 + color1[1] * weight1,
        color0[2] * weight0 + color1[2] * weight1,
        color0[3] * weight0 + color1[3] * weight1);

    return OK;
  };
}


ColorMap color_map_steps(std::vector<std::pair<double, Color>> steps) {
  std::sort(
      steps.begin(),
      steps.end(),
      [] (const auto& a, const auto& b) {
        return a.first < b.first;
      });

  return [steps] (const auto& value, Color* color) -> ReturnCode {
    if (steps.empty()) {
      *color = Color{};
      return OK;
    }

    double value_num  = 0.0;
    try {
      value_num = std::clamp(std::stod(value), 0.0, 1.0);
    } catch (...) {
      return errorf(
          ERROR,
          "invalid data; can't map '{}' to a color",
          value);
    }

    auto step = std::upper_bound(
        steps.begin(),
        steps.end(),
        value_num,
        [] (const auto& a, const auto& b) {
          return a < b.first;
        });

    if (step == steps.begin()) {
      *color = {};
    } else {
      *color = (step - 1)->second;
    }

    return OK;
  };
}

} // namespace clip

