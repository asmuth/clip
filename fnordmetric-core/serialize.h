/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_SERIALIZE_H
#define _FNORDMETRIC_SERIALIZE_H

#include <stdlib.h>
#include <stdint.h>
#include <vector>

namespace fnordmetric {
namespace serialize {

template<typename T>
void toBytes(const T& value, std::vector<uint8_t>* destination);

template<typename T>
void toBytesV(std::vector<uint8_t>* destination, T value) {
  toBytes(value, destination);
}

template<typename T, typename... D>
void toBytesV(std::vector<uint8_t>* destination, T value, D... tail) {
  toBytes(value, destination);
  toBytesV(destination, tail...);
}

template<typename T>
T fromBytes(std::vector<uint8_t>::const_iterator* cursor);

}
}

#endif
