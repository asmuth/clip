/**
 * Copyright (c) 2016, Paul Asmuth <paul@asmuth.com>
 * All rights reserved.
 * 
 * This file is part of the "libzdb" project. libzdb is free software licensed
 * under the 3-Clause BSD License (BSD-3-Clause).
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../core/util/exception.h"
#include "../core/util/time.h"
#include "../core/zdb.h"
#include "unittest.h"

UNIT_TEST(ZDBTest);

TEST_CASE(ZDBTest, TestCreateAndInsert, [] () {
  unlink("/tmp/__test.zdb");
  auto t0 = WallClock::unixMicros();

  zdb::database_ref db;
  EXPECT_SUCCESS(zdb::open("/tmp/__test.zsb", ZDB_OPEN_DEFAULT, &db));
  EXPECT_SUCCESS(zdb::table_add(db, "mytbl"));
  int col_time;
  EXPECT_SUCCESS(zdb::column_add(db, "mytbl", "time", ZDB_UINT64, &col_time));
  int col_val;
  EXPECT_SUCCESS(zdb::column_add(db, "mytbl", "val", ZDB_UINT64, &col_val));

  std::vector<const void*> tuple(std::max(col_val, col_time) + 1);
  std::vector<size_t> tuple_size(std::max(col_val, col_time) + 1);
  tuple_size[col_time] = sizeof(uint64_t);
  tuple_size[col_val] = sizeof(uint64_t);

  for (uint64_t i = 0; i < 100000; ++i) {
    uint64_t time = t0 + 20 * i;
    tuple[col_time] = &time;
    tuple[col_val] = &i;

    EXPECT_SUCCESS(
        zdb::put_raw(db, "mytbl", tuple.data(), tuple_size.data(), tuple.size()));
  }
});


