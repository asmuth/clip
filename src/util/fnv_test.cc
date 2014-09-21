/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
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
