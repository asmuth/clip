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
#include "measure.h"
#include "layer.h"
#include <assert.h>
#include <iostream>

namespace clip {

Measure::Measure() : Measure(Unit::UNIT, 0) {}

Measure::Measure(Unit u, double v) : unit(u), value(v) {}

Measure::operator double() const {
  return value;
}

Measure from_unit(double v) {
  return Measure(Unit::UNIT, v);
}

Measure from_mm(double v) {
  return Measure(Unit::MM, v);
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
    return errorf(ERROR, "invalid number: '{}'", s);
  }

  if (unit_pos == s.size()) {
    *measure = from_user(value);
    return OK;
  }

  auto unit = s.substr(unit_pos);
  if (unit == "mm") {
    *measure = from_mm(value);
    return OK;
  }

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

  return errorf(
      ERROR,
      "invalid unit: '{}', expected one of 'mm', 'px', 'pt', 'em', 'rem', 'rel' or '%'",
      unit);
}

std::string measure_to_string(const Measure& m) {
  switch (m.unit) {
    case Unit::UNIT:
    case Unit::USER:
      return fmt::format("{}", m.value);
    case Unit::MM:
      return fmt::format("{}mm", m.value);
    case Unit::PX:
      return fmt::format("{}px", m.value);
    case Unit::PT:
      return fmt::format("{}pt", m.value);
    case Unit::REM:
      return fmt::format("{}rem", m.value);
    case Unit::REL:
      return fmt::format("{}%", m.value * 100);
    default:
      break;
  }

  return "???";
}

Measure measure_or(const Measure& primary, const Measure& fallback) {
  if (primary == 0) {
    return fallback;
  } else {
    return primary;
  }
}

void convert_unit(
    const Layer& layer,
    Measure* measure) {
  convert_unit_typographic(layer.dpi, layer.font_size, measure);
}

Measure convert_unit(
    const Layer& layer,
    Measure measure) {
  convert_unit(layer, &measure);
  return measure;
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

void convert_unit(
    const std::vector<UnitConverter>& converters,
    Measure* measure) {
  for (const auto& c : converters) {
    c(measure);
  }
}

void convert_unit_typographic(
    double dpi,
    Measure font_size,
    Measure* measure) {
  switch (font_size.unit) {
    case Unit::PT:
      font_size.value = (double(font_size) / 72.0) * dpi;
      break;
    case Unit::UNIT:
      break;
  }

  switch (measure->unit) {
    case Unit::MM:
      measure->value = (measure->value / 25.4) * dpi;
      measure->unit = Unit::UNIT;
      break;
    case Unit::PT:
      measure->value = (measure->value / 72.0) * dpi;
      measure->unit = Unit::UNIT;
      break;
    case Unit::REM:
      measure->value = measure->value * font_size;
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
      measure->value = measure->value * range;
      break;
  }
}

void convert_unit_user(
    std::function<double (double)> converter,
    Measure* measure) {
  switch (measure->unit) {
    case Unit::USER:
      measure->unit = Unit::REL;
      measure->value = converter(measure->value);
      break;
  }
}

void measure_normalize(const MeasureConv& conv, Measure* measure) {
  double parent_size = 0;
  switch (conv.parent_size.unit) {
    case Unit::PT:
      parent_size = (double(conv.parent_size) / 72.0) * conv.dpi;
      break;
    case Unit::REM:
    case Unit::REL:
      parent_size = 0;
      break;
    case Unit::UNIT:
    case Unit::PX:
    case Unit::USER:
      parent_size = double(conv.parent_size);
      break;
  }

  double font_size = 0;
  switch (conv.font_size.unit) {
    case Unit::PT:
      font_size = (double(conv.font_size) / 72.0) * conv.dpi;
      break;
    case Unit::REM:
      font_size = 0;
      break;
    case Unit::REL:
      font_size = parent_size * double(conv.font_size);
      break;
    case Unit::UNIT:
    case Unit::PX:
    case Unit::USER:
      font_size = double(conv.font_size);
      break;
  }

  switch (measure->unit) {
    case Unit::PT:
      measure->value = (measure->value / 72.0) * conv.dpi;
      measure->unit = Unit::UNIT;
      break;
    case Unit::REM:
      measure->value = measure->value * font_size;
      measure->unit = Unit::UNIT;
      break;
    case Unit::REL:
      measure->value = measure->value * parent_size;
      measure->unit = Unit::UNIT;
      break;
    case Unit::UNIT:
    case Unit::PX:
    case Unit::USER:
      measure->unit = Unit::UNIT;
      break;
  }
}

void measure_normalizev(
    const MeasureConv& conv,
    Measure* begin,
    Measure* end) {
  for (; begin != end; ++begin) {
    measure_normalize(conv, begin);
  }
}

} // namespace clip

