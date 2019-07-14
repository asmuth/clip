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
#include "measure.h"
#include <assert.h>
#include <iostream>

namespace fviz {

Measure::Measure() : Measure(Unit::UNIT, 0) {}

Measure::Measure(Unit u, double v) : unit(u), value(v) {}

Measure::operator double() const {
  return value;
}

Measure from_unit(double v) {
  return Measure(Unit::UNIT, v);
}

Measure from_px(double v) {
  return Measure(Unit::PX, v);
}

Measure from_pt(double v) {
  return Measure(Unit::PT, v);
}

Measure from_pt(double v, double dpi) {
  return Measure(Unit::UNIT, (v / 72.0) * dpi);
}

Measure from_em(double v, double font_size) {
  return Measure(Unit::UNIT, v * font_size);
}

Measure from_em(double v) {
  return Measure(Unit::REM, v);
}

Measure from_user(double v) {
  return Measure(Unit::USER, v);
}

Measure from_rel(double v) {
  return Measure(Unit::REL, v);
}

ReturnCode parse_measure(
    const std::string& s,
    Measure* measure) {
  double value;
  size_t unit_pos;
  try {
    value = std::stod(s, &unit_pos);
  } catch (... ) {
    return ERROR;
  }

  if (unit_pos == s.size()) {
    *measure = from_user(value);
    return OK;
  }

  auto unit = s.substr(unit_pos);
  if (unit == "px") {
    *measure = from_px(value);
    return OK;
  }

  if (unit == "pt") {
    *measure = from_pt(value);
    return OK;
  }

  if (unit == "em" || unit == "rem") {
    *measure = from_em(value);
    return OK;
  }

  if (unit == "%") {
    *measure = from_rel(value / 100.0);
    return OK;
  }

  if (unit == "rel") {
    *measure = from_rel(value);
    return OK;
  }

  return {
    ERROR,
    StringUtil::format(
        "invalid unit: '$0', expected one of 'px', 'pt', 'em', 'rem', 'rel' or '%'",
        unit)
  };
}

Measure measure_or(const Measure& primary, const Measure& fallback) {
  if (primary == 0) {
    return fallback;
  } else {
    return primary;
  }
}

void convert_units(
    const std::vector<UnitConverter>& converters,
    Measure* begin,
    Measure* end) {
  assert(begin <= end);

  for (auto cur = begin; cur != end; ++cur) {
    for (const auto& c : converters) {
      c(cur);
    }
  }
}

void convert_unit_typographic(
    double dpi,
    double font_size,
    Measure* measure) {
  switch (measure->unit) {
    case Unit::PT:
      measure->value = (measure->value / 72.0) * dpi;
      measure->unit = Unit::UNIT;
      break;
    case Unit::REM:
      measure->value = (measure->value / 72.0) * dpi * font_size;
      measure->unit = Unit::UNIT;
      break;
  }
}

void convert_unit_relative(
    double range,
    Measure* measure) {
  switch (measure->unit) {
    case Unit::REL:
      measure->unit = Unit::UNIT;
      measure->value = std::clamp(measure->value, 0.0, 1.0) * range;
      break;
  }
}

void convert_unit_user(
    std::function<double (double)> converter,
    Measure* measure) {
  switch (measure->unit) {
    case Unit::USER:
      measure->unit = Unit::REL;
      measure->value = std::clamp(converter(measure->value), 0.0, 1.0);
      break;
  }
}

} // namespace fviz

