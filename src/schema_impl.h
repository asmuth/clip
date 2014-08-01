/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _FNORDMETRIC_SCHEMA_IMPL_H
#define _FNORDMETRIC_SCHEMA_IMPL_H

#include <stdlib.h>
#include <vector>

namespace fnordmetric {

template<typename... T>
TypedSchema<T...>::TypedSchema(const T... fields) : 
    Schema(unpackFields(fields...)) {}

// FIXPAUL there must be some better way to do this...
template<typename... T>
std::vector<Field> TypedSchema<T...>::unpackFields(T... fields) {
  std::vector<Field> unpacked;
  const Field packed[] = {fields...};
  int num_fields = sizeof(packed) / sizeof(Field);

  for (int i = 0; i < num_fields; ++i) {
    unpacked.push_back(packed[i]);
  }

  return unpacked;
};

}
#endif
