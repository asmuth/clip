/**
 * This file is part of the "clip" project
 *   Copyright (c) 2020 Paul Asmuth
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
#include "units.h"
#include "layer.h"

namespace clip {

Number Number::operator*(double v) const {
  return value * v;
}

bool unit_parse(const std::string& s, Unit* u) {
  static const std::unordered_map<std::string, Unit> unit_map = {
    {"mm", Unit::MM},
    {"px", Unit::PX},
    {"pt", Unit::PT},
    {"em", Unit::EM},
    {"rem", Unit::REM},
    {"%", Unit::PERCENT},
  };

  auto unit_iter = unit_map.find(s);
  if (unit_iter != unit_map.end()) {
    *u = unit_iter->second;
    return true;
  } else {
    return false;
  }
}

Number unit_from_mm(double v, double dpi) {
  return (v / 25.4) * dpi;
}

Number unit_from_pt(double v, double dpi) {
  return (v / 72.0) * dpi;
}

Number unit_from_px(double v) {
  return v;
}

Number unit_from_percent(double v, double base) {
  return (v / 100.0) * base;
}

} // namespace clip

