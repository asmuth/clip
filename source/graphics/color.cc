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
#include <string.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <utils/stringutil.h>
#include "color.h"

namespace plotfx {

Color Color::fromRGBA(double red, double green, double blue, double alpha) {
  return Color({ red, green, blue, alpha });
}

Color Color::fromRGB(double red, double green, double blue) {
  return Color({ red, green, blue, 1.0 });
}

Color::Color() {
  memset(components_, 0, sizeof(components_));
}

Color::Color(
    const std::initializer_list<double>& components) {
  assert(components.size() <= kMaxComponents);

  size_t i = 0;
  for (auto c : components) {
    components_[i++] = c;
  }
}

double Color::red() const {
  return components_[0];
}

double Color::green() const {
  return components_[1];
}

double Color::blue() const {
  return components_[2];
}

double Color::alpha() const {
  return components_[3];
}

double Color::component(size_t idx) const {
  return components_[idx];
}

double Color::operator[](size_t idx) const {
  return components_[idx];
}

double& Color::operator[](size_t idx) {
  return components_[idx];
}

bool Color::parse(const std::string& str) {
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

bool Color::parseHex(const std::string& str) {
  if (str.size() != 7) {
    return false;
  }

  components_[0] = std::stoi(str.substr(1, 2), nullptr, 16) / 255.0f;
  components_[1] = std::stoi(str.substr(3, 2), nullptr, 16) / 255.0f;
  components_[2] = std::stoi(str.substr(5, 2), nullptr, 16) / 255.0f;
  components_[3] = 1.0f;
  return true;
}

bool Color::parseHexAlpha(const std::string& str) {
  if (str.size() != 9) {
    return false;
  }

  components_[0] = std::stoi(str.substr(1, 2), nullptr, 16) / 255.0f;
  components_[1] = std::stoi(str.substr(3, 2), nullptr, 16) / 255.0f;
  components_[2] = std::stoi(str.substr(5, 2), nullptr, 16) / 255.0f;
  components_[3] = std::stoi(str.substr(7, 2), nullptr, 16) / 255.0f;
  return true;
}

bool Color::parseHexShort(const std::string& str) {
  if (str.size() != 4) {
    return false;
  }

  components_[0] = std::stoi(str.substr(1, 1), nullptr, 16) / 16.0f;
  components_[1] = std::stoi(str.substr(2, 1), nullptr, 16) / 16.0f;
  components_[2] = std::stoi(str.substr(3, 1), nullptr, 16) / 16.0f;
  components_[3] = 1.0f;
  return true;
}

std::string Color::to_hex_str() const {
  std::stringstream ss;
  ss
    << "#"
    << std::hex
    << std::setfill('0')
    << std::setw(2)
    << int(components_[0] * 255)
    << std::setfill('0')
    << std::setw(2)
    << int(components_[1] * 255)
    << std::setfill('0')
    << std::setw(2)
    << int(components_[2] * 255);

  return ss.str();
}

std::ostream& operator <<(std::ostream& os, const Color& c) {
  os << "Color(";
  os << c[0];
  os << ", ";
  os << c[1];
  os << ", ";
  os << c[2];
  os << ", ";
  os << c[3];
  os << ")";
  return os;
}

} // namespace plotfx

