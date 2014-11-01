/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNOR_UTIL_RANDOM_H_
#define _FNOR_UTIL_RANDOM_H_
#include <stdlib.h>
#include <stdint.h>
#include <string>

namespace fnord {
namespace util {

class Random {
public:

  /**
   * Call srand()
   */
  static void init();

  /**
   * Create a new random alphanumeric string
   */
  static std::string alphanumericString(int nchars);

};

}
}
#endif
