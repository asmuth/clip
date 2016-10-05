/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include "util/exception.h"
#include "util/unittest.h"
#include "util/time.h"
#include "tsdb.h"

UNIT_TEST(TSDBTest);

TEST_CASE(TSDBTest, TestCreateAndInsert, [] () {
  tsdb::TSDB db;

  auto t0 = WallClock::unixMicros();

  EXPECT(db.createSeries(1, tsdb::PageType::UINT64) == true);
  for (size_t i = 0; i < 100000; ++i) {
    EXPECT(db.insertUInt64(1, t0 + 20 * i, i) == true);
  }

  tsdb::Cursor cursor(tsdb::PageType::UINT64);
  EXPECT(db.getCursor(1, &cursor) == true);

  for (size_t i = 0; i < 100000; ++i) {
    uint64_t ts;
    uint64_t value;
    EXPECT(cursor.next(&ts, &value) == true);
    EXPECT(ts == t0 + 20 * i);
    EXPECT(value == i);
  }
});

