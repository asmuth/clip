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
#pragma once
#include <stdlib.h>
#include <vector>
#include <string>
#include <return_code.h>

namespace clip {
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

struct MeasureConv {
  double dpi;
  Measure font_size;
  Measure parent_size;
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
    Measure font_size,
    Measure* measure);

void convert_unit_relative(
    double range,
    Measure* measure);

void convert_unit_user(
    std::function<double (double)> converter,
    Measure* measure);

void measure_normalize(const MeasureConv& conv, Measure* measure);

void measure_normalizev(
    const MeasureConv& conv,
    Measure* begin,
    Measure* end);

} // namespace clip

