/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "fnv.h"

void testSimpleHashing() {
  fnordmetric::FNV<uint64_t> fnv64;
  fnordmetric::FNV<uint32_t> fnv32;

  assert(fnv64.hash("fnord") == 0xE4D8CB6A3646310);
  assert(fnv32.hash("fnord") == 0x6D964EB0);
}


int main() {
  testSimpleHashing();
}
