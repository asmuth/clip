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

  EXPECT(db.createSeries(1, tsdb::PageType::UINT64) == true);
  EXPECT(db.insertUInt64(1, WallClock::unixMicros(), 123) == true);
  EXPECT(db.insertUInt64(1, WallClock::unixMicros(), 456) == true);
  EXPECT(db.insertUInt64(1, WallClock::unixMicros(), 789) == true);

});

