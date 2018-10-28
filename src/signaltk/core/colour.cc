/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * signaltk is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include "colour.h"

namespace signaltk {

Colour Colour::fromRGBA(double red, double green, double blue, double alpha) {
  return Colour({ red, green, blue, alpha });
}

Colour Colour::fromRGB(double red, double green, double blue) {
  return Colour({ red, green, blue, 1.0 });
}

Colour::Colour(
    const std::initializer_list<double>& components) {
  assert(components.size() <= kMaxComponents);

  size_t i = 0;
  for (auto c : components) {
    components_[i++] = c;
  }
}

double Colour::red() const {
  return components_[0];
}

double Colour::green() const {
  return components_[1];
}

double Colour::blue() const {
  return components_[2];
}

double Colour::alpha() const {
  return components_[3];
}

double Colour::component(size_t idx) const {
  return components_[idx];
}

double Colour::operator[](size_t idx) const {
  return components_[idx];
}

double& Colour::operator[](size_t idx) {
  return components_[idx];
}

} // namespace signaltk

