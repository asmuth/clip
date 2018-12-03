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

const Property& Property::operator[](size_t i) const {
  assert(next);
  assert(i < next->size());
  return (*next)[i];
}

size_t Property::size() const {
  if (next) {
    return next->size();
  } else {
    return 0;
  }
}

Property::operator const std::string&() const {
  return value;
}

} // namespace plist

