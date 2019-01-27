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
#pragma once
#include <stdlib.h>
#include <vector>
#include <string>
#include <utils/return_code.h>

namespace plotfx {
struct Rectangle;

enum class Unit {
  UNIT,  // Screen units
  PX,    // Pixels
  PT,    // Typographic Points
  REM,   // Typographic "em" size
  REL,   // Relative to the enclosing element (0..1),
  USER,  // User units on arbitrary scale
};

struct Measure {
  Measure();
  explicit Measure(Unit unit, double value);
  Unit unit;
  double value;
  operator double() const;
};

Measure from_unit(double v);
Measure from_px(double v);
Measure from_pt(double v, double dpi);
Measure from_pt(double v);
Measure from_em(double v, double font_size);
Measure from_em(double v);
Measure from_rel(double v);
Measure from_user(double v);

ReturnCode parse_measure(
    const std::string& s,
    Measure* value);

Measure measure_or(const Measure& primary, const Measure& fallback);

using UnitConverter = std::function<void (Measure*)>;

void convert_units(
    const std::vector<UnitConverter>& converters,
    Measure* begin,
    Measure* end);

void convert_unit_typographic(
    double dpi,
    double font_size_pt,
    Measure* measure);

void convert_unit_relative(
    double range_begin,
    double range_end,
    Measure* measure);

} // namespace plotfx

