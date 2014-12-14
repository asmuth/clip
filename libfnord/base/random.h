/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORD_RANDOM_H_
#define _FNORD_RANDOM_H_
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <random>

namespace fnord {

class Random {
public:

  Random();

  /**
   * Return 64 bits of random data
   */
  uint64_t random64();

  /**
   * Return 64 bits of random data as hex chars
   */
  std::string hex64();

  /**
   * Return 128 bits of random data as hex chars
   */
  std::string hex128();

  /**
   * Return 256 bits of random data as hex chars
   */
  std::string hex256();

  /**
   * Return 512 bits of random data as hex chars
   */
  std::string hex512();

  /**
   * Create a new random alphanumeric string
   */
  std::string alphanumericString(int nchars);

protected:
  std::mt19937_64 prng_;
};

}
#endif
