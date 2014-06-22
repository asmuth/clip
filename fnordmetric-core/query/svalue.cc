/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <assert.h>
#include "svalue.h"

namespace fnordmetric {
namespace query {


SValue::SValue(int64_t integer_value) : type_(T_INTEGER) {
  data_.t_integer = integer_value;
}

SValue::SValue(double float_value) : type_(T_FLOAT) {
  data_.t_float = float_value;
}

}
}
