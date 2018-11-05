/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * signaltk is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <string>

namespace signaltk {

class Colour {
public:
  static const size_t kMaxComponents = 4;

  static Colour fromRGB(double red, double green, double blue);
  static Colour fromRGBA(double red, double green, double blue, double alpha);

  Colour();
  Colour(const std::initializer_list<double>& components);

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

protected:
  double components_[kMaxComponents];
};

} // namespace signaltk

