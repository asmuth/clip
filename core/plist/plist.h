/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <memory>
#include <string>
#include <vector>

namespace plist {
struct Property;

using PropertyList = std::vector<Property>;

struct PropertyValue {
  std::string data;
  bool is_literal;
  operator const std::string&() const;
};

struct Property {
  std::string name;
  std::vector<PropertyValue> values;
  std::unique_ptr<std::vector<Property>> child;

  const PropertyValue& get(size_t i) const;
  const PropertyValue& operator[](size_t i) const;
  size_t size() const;
};

} // namespace plist

