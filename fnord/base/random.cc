/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <assert.h>
#include <fnord/base/inspect.h>
#include <fnord/base/random.h>
#include <fnord/base/stringutil.h>

namespace fnord {

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
