/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2018 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _FNORD_RANDOM_H_
#define _FNORD_RANDOM_H_
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <random>

namespace fviz {

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
