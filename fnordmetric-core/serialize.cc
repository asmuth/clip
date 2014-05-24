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

template<>
void toBytes<int64_t>(const int64_t& value, std::vector<uint8_t>* destination) {
  printf("serialize int..\n");
}

template<>
void toBytes<double>(const double& value, std::vector<uint8_t>* destination) {
  printf("serialize double..\n");
}

}
}
