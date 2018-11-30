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
#include <iostream>

namespace plotfx {

Measure::Measure() : Measure(Unit::UNIT, 0.0f) {}

Measure::Measure(
    Unit _unit,
    double _value) :
    unit(_unit),
    value(_value) {}

Measure::operator double() const {
  return value;
}

Measure from_unit(double v) {
  return Measure{.unit = Unit::UNIT, .value = v};
}

Measure from_px(double v) {
  return Measure{.unit = Unit::PX, .value = v};
}

Measure from_pt(double v) {
  return Measure{.unit = Unit::PT, .value = v};
}

Measure from_rem(double v) {
  return Measure{.unit = Unit::REM, .value = v};
}

Measure to_px(const MeasureTable& t, const Measure& v) {
  double v_px;

  switch (v.unit) {
    case Unit::UNIT:
    case Unit::PX:
      v_px = v.value;
      break;
    case Unit::PT:
      v_px = (v.value / 72.0) * t.dpi;
      break;
    case Unit::REM:
      v_px = ((v.value * t.rem) / 72.0) * t.dpi;
      break;
  }

  return Measure{.unit = Unit::PX, .value = v_px};
}

ReturnCode parse_measure(const std::string& s, Measure* measure) {
  double value;
  size_t unit_pos;
  try {
    value = std::stod(s, &unit_pos);
  } catch (... ) {
    return ERROR_INVALID_ARGUMENT;
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

  if (unit == "rem") {
    *measure = from_rem(value);
    return OK;
  }

  return ERROR_INVALID_ARGUMENT;
}

} // namespace plotfx

