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

Measure::Measure() : Measure(0) {}

Measure::Measure(double v) : value(v) {}

Measure::operator double() const {
  return value;
}

Measure from_unit(double v) {
  return Measure(v);
}

Measure from_px(double v) {
  return Measure(v);
}

Measure from_pt(double v, double dpi) {
  return Measure((v / 72.0) * dpi);
}

Measure from_em(double v, double font_size) {
  return Measure(v * font_size);
}

ReturnCode parse_measure_abs(
    const std::string& s,
    double dpi,
    Measure* measure) {
  double value;
  size_t unit_pos;
  try {
    value = std::stod(s, &unit_pos);
  } catch (... ) {
    return ERROR;
  }

  auto unit = s.substr(unit_pos);
  if (unit == "px") {
    *measure = from_px(value);
    return OK;
  }

  if (unit == "pt") {
    *measure = from_pt(value, dpi);
    return OK;
  }

  return ERROR;
}

ReturnCode parse_measure_rel(
    const std::string& s,
    double dpi,
    double font_size,
    Measure* measure) {
  double value;
  size_t unit_pos;
  try {
    value = std::stod(s, &unit_pos);
  } catch (... ) {
    return ERROR;
  }

  auto unit = s.substr(unit_pos);
  if (unit == "px") {
    *measure = from_px(value);
    return OK;
  }

  if (unit == "pt") {
    *measure = from_pt(value, dpi);
    return OK;
  }

  if (unit == "em" || unit == "rem") {
    *measure = from_em(value, font_size);
    return OK;
  }

  return ERROR;
}

Measure measure_or(const Measure& primary, const Measure& fallback) {
  if (primary == 0) {
    return fallback;
  } else {
    return primary;
  }
}

} // namespace plotfx

