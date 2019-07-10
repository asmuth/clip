/**
 * This file is part of the "plotfx" project
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
#include "measure.h"
#include <assert.h>
#include <iostream>

namespace plotfx {

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

} // namespace plotfx

