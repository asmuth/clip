/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/util/random.h>

namespace fnord {
namespace util {

void Random::init() {
  srand(time(NULL));
}

std::string Random::alphanumericString(int nchars) {
  static const char kAlphanumericChars[] =
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  std::string str;
  for (int i = 0; i < nchars; ++i) {
    // FIXPAUL too many rand() calls!
    str += kAlphanumericChars[rand() % (sizeof(kAlphanumericChars) - 1)];
  }

  return str;
}

}
}

