/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "format.h"

namespace fnordmetric {
namespace format {

std::string numberToHuman(double value) {
  char buf[256];
  auto len = snprintf(buf, sizeof(buf), "%f", value);
  return std::string(buf, len);
}

std::string svalueToHuman(const query::SValue& value) {
  return value.toString();
}


}
}
