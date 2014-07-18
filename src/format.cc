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
#include "math.h"
#include "format.h"

namespace fnordmetric {
namespace format {

std::string numberToHuman(double value) {
  char buf[256];
  size_t len;

  auto abs_value = fabs(value);

  if (abs_value < 1){
    len = snprintf(buf, sizeof(buf), "%.2f", value);
  }

  else if (abs_value < 10){
    len = snprintf(buf, sizeof(buf), "%.1f", value);
  }

  else if (abs_value < 100) {
    len = snprintf(buf, sizeof(buf), "%.0f", value);
  }

  else if (abs_value > 1000) {
    len = snprintf(buf, sizeof(buf), "%.1fk", value / 1000);
  }

  return std::string(buf, len);
}

std::string svalueToHuman(const query::SValue& value) {
  return value.toString();
}


}
}
