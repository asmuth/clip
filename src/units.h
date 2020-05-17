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
#pragma once
#include <stdlib.h>
#include <vector>
#include <string>
#include <return_code.h>
#include "utils/option.h"

namespace clip {
struct Layer;
struct Rectangle;

enum class Unit {
  UNIT,  // Screen units
  MM,    // Millimeters
  PX,    // Pixels
  PT,    // Typographic Points
  EM,   // Typographic "em" size
  REM,   // Typographic root "em" size
  REL,   // Relative to the enclosing element (0..1),
  PERCENT, // Percent (0..100)
  USER,  // User units on arbitrary scale
};

/**
 * A unitless number that was normalized to the respective internal coordinate
 * system
 */
struct Number {
  Number() : value(0) {}
  Number(double v) : value(v) {}
  double value;
  Number operator*(double v) const;
};

/**
 * A vector of two unitless numbers
 */
using Vector2 = std::array<Number, 2>;

/**
 * A number expressed in a specific unit
 */
template <Unit U>
struct NumberT {
  NumberT() : value(0) {}
  NumberT(double v) : value(v) {}
  NumberT<U> operator*(double v) const;
  static const Unit unit = U;
  double value;
};

/**
 * A wrapper that contains either an absolute number or a scalar relative to
 * another value of the same unit
 */
template <Unit U>
struct NumberRel {
  Option<NumberT<U>> value;
  Option<double> value_rel;
};

/**
 * A value expressed in typographic points. 1pt = 1/72 inch
 */
using NumberPT = NumberT<Unit::PT>;

/**
 * A value expressed in pixels
 */
using NumberPX = NumberT<Unit::PX>;

/**
 * A value expressed in millimeters
 */
using NumberMM = NumberT<Unit::MM>;

/**
 * A function that takes normalizes a value
 */
using UnitConv = std::function<Number (double)>;

/**
 * A table of unit conversions
 */
using UnitConvMap = std::unordered_map<Unit, UnitConv>;

/**
 * Resolve a relative unit to an absolute value
 */
template <Unit U>
NumberT<U> unit_resolve(const NumberRel<U>& v, const NumberT<U>& r);

bool unit_parse(const std::string& s, Unit* u);

Number unit_from_mm(double v, double dpi);
Number unit_from_pt(double v, double dpi);
Number unit_from_px(double v);
Number unit_from_percent(double v, double base);

} // namespace clip

