/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "serialize.h"

namespace fnordmetric {
namespace serialize {

// FIXPAUL
template<>
void toBytes<int64_t>(const int64_t& value, std::vector<uint8_t>* destination) {
  int64_t local_value = value;
  uint8_t bytes[4];

  memcpy(bytes, &local_value, 4);

  destination->emplace_back(INT64);
  destination->emplace_back(bytes[0]);
  destination->emplace_back(bytes[1]);
  destination->emplace_back(bytes[2]);
  destination->emplace_back(bytes[3]);
}

// FIXPAUL
template<>
void toBytes<double>(const double& value, std::vector<uint8_t>* destination) {
  toBytes((int64_t) -42, destination);
  //destination->emplace_back(IEE754);
}

}
}
