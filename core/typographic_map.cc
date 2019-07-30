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
#include "typographic_map.h"

namespace fviz {

MeasureMap measure_map_linear(const Measure& min, const Measure& max) {
  return [min, max] (const std::string& value, Measure* measure) -> ReturnCode {
    double value_num  = 0.0;
    try {
      value_num = std::clamp(std::stod(value), 0.0, 1.0);
    } catch (...) {
      return errorf(
          ERROR,
          "invalid data; can't map '{}' to a typographic unit",
          value);
    }

    *measure = from_unit(double(min) + (double(max) - double(min)) * value_num);
    return OK;
  };
}

} // namespace fviz

