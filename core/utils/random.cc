/**
 * This file is part of the "clip" project
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
#include <assert.h>
#include "random.h"
#include "stringutil.h"

namespace clip {

Random::Random() {
  std::random_device r;
  prng_.seed(r());
}

uint64_t Random::random64() {
  uint64_t rval = prng_();
  assert(rval > 0);
  return rval;
}

std::string Random::hex64() {
  uint64_t val = random64();
  return StringUtil::hexPrint(&val, sizeof(val), false);
}

std::string Random::hex128() {
  uint64_t val[2];
  val[0] = random64();
  val[1] = random64();

  return StringUtil::hexPrint(&val, sizeof(val), false);
}

std::string Random::hex256() {
  uint64_t val[4];
  val[0] = random64();
  val[1] = random64();
  val[2] = random64();
  val[3] = random64();

  return StringUtil::hexPrint(&val, sizeof(val), false);
}

std::string Random::hex512() {
  uint64_t val[8];
  val[0] = random64();
  val[1] = random64();
  val[2] = random64();
  val[3] = random64();
  val[4] = random64();
  val[5] = random64();
  val[6] = random64();
  val[7] = random64();

  return StringUtil::hexPrint(&val, sizeof(val), false);
}

std::string Random::alphanumericString(int nchars) {
  static const char kAlphanumericChars[] =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  std::string str;
  // FIXPAUL too many rand() calls!
  for (int i = 0; i < nchars; ++i) {
    str += kAlphanumericChars[prng_() % (sizeof(kAlphanumericChars) - 1)];
  }

  return str;
}

}
