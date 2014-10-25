/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/ieee754.h>

namespace fnord {
namespace util {

uint64_t IEEE754::toBytes(double value) {
  return value;
}

double IEEE754::fromBytes(uint64_t bytes) {
  return bytes;
}

}
}

