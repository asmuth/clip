/**
 * This file is part of the "fviz" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <assert.h>
#include "inspect.h"
#include "random.h"
#include "stringutil.h"

namespace fviz {

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
