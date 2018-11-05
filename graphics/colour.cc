/**
 * This file is part of the "signaltk" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * signaltk is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <string.h>
#include <utils/stringutil.h>
#include "colour.h"

namespace signaltk {

Colour Colour::fromRGBA(double red, double green, double blue, double alpha) {
  return Colour({ red, green, blue, alpha });
}

Colour Colour::fromRGB(double red, double green, double blue) {
  return Colour({ red, green, blue, 1.0 });
}

Colour::Colour() {
  memset(components_, 0, sizeof(components_));
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

bool Colour::parse(const std::string& str) {
  if (StringUtil::beginsWith(str, "#")) {
    switch (str.size()) {
      case 4:
        return parseHexShort(str);
      case 7:
        return parseHex(str);
      case 9:
        return parseHexAlpha(str);
    }
  }

  return false;
}

bool Colour::parseHex(const std::string& str) {
  if (str.size() != 7) {
    return false;
  }

  components_[0] = std::stoi(str.substr(1, 2), nullptr, 16) / 255.0f;
  components_[1] = std::stoi(str.substr(3, 2), nullptr, 16) / 255.0f;
  components_[2] = std::stoi(str.substr(5, 2), nullptr, 16) / 255.0f;
  components_[3] = 1.0f;
  return true;
}

bool Colour::parseHexAlpha(const std::string& str) {
  if (str.size() != 9) {
    return false;
  }

  components_[0] = std::stoi(str.substr(1, 2), nullptr, 16) / 255.0f;
  components_[1] = std::stoi(str.substr(3, 2), nullptr, 16) / 255.0f;
  components_[2] = std::stoi(str.substr(5, 2), nullptr, 16) / 255.0f;
  components_[3] = std::stoi(str.substr(7, 2), nullptr, 16) / 255.0f;
  return true;
}

bool Colour::parseHexShort(const std::string& str) {
  if (str.size() != 4) {
    return false;
  }

  components_[0] = std::stoi(str.substr(1, 1), nullptr, 16) / 16.0f;
  components_[1] = std::stoi(str.substr(2, 1), nullptr, 16) / 16.0f;
  components_[2] = std::stoi(str.substr(3, 1), nullptr, 16) / 16.0f;
  components_[3] = 1.0f;
  return true;
}

} // namespace signaltk

