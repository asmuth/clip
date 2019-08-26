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
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <array>
#include <vector>
#include <string>

namespace clip {

class Color {
public:
  static const size_t kMaxComponents = 4;

  static Color fromRGB(double red, double green, double blue);
  static Color fromRGBA(double red, double green, double blue, double alpha);

  Color();
  Color(const std::initializer_list<double>& components);

  double red() const;
  double green() const;
  double blue() const;
  double alpha() const;

  double component(size_t idx) const;
  double operator[](size_t idx) const;
  double& operator[](size_t idx);

  bool parse(const std::string& str);
  bool parseHex(const std::string& str);
  bool parseHexAlpha(const std::string& str);
  bool parseHexShort(const std::string& str);

  std::string to_hex_str(size_t n = 3) const;

protected:
  std::array<double, kMaxComponents> components_;
};

std::ostream& operator <<(std::ostream& os, const Color& c);

} // namespace clip

