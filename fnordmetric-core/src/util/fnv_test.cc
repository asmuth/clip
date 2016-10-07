/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <metricd/util/fnv.h>
#include <metricd/util/unittest.h>

UNIT_TEST(FNVTest);

TEST_CASE(FNVTest, TestFNV64, [] () {
  fnord::util::FNV<uint64_t> fnv64;
  EXPECT_EQ(fnv64.hash("fnord"), 0xE4D8CB6A3646310);
});

TEST_CASE(FNVTest, TestFNV32, [] () {
  fnord::util::FNV<uint32_t> fnv32;
  EXPECT_EQ(fnv32.hash("fnord"), 0x6D964EB0);
});
