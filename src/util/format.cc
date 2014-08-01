/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "math.h"
#include "format.h"

namespace fnordmetric {
namespace util {
namespace format {

std::string numberToHuman(double value) {
  char buf[256];
  size_t len = 0;

  auto abs_value = fabs(value);

  if (abs_value == 0){
    len = snprintf(buf, sizeof(buf), "0");
  }

  else if (abs_value < 1){
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

  else {
    len = snprintf(buf, sizeof(buf), "%.0f", value);
  }

  return std::string(buf, len);
}

}
}
}
