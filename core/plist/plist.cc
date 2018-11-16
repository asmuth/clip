/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include "plist.h"

namespace plist {

PropertyValue::operator const std::string&() const {
  return data;
}

const PropertyValue& Property::get(size_t i) const {
  assert(i < values.size());
  return values[i];
}

const PropertyValue& Property::operator[](size_t i) const {
  assert(i < values.size());
  return values[i];
}

size_t Property::size() const {
  return values.size();
}

} // namespace plist

