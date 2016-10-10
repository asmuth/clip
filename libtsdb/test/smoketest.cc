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
#include <unistd.h>
#include "metricd/util/exception.h"
#include "metricd/util/unittest.h"
#include "metricd/util/time.h"
#include "tsdb.h"

UNIT_TEST(TSDBTest);

TEST_CASE(TSDBTest, TestCreateAndInsert, [] () {
  unlink("/tmp/__test.tsdb");

  auto t0 = WallClock::unixMicros();

  {
    std::unique_ptr<tsdb::TSDB> db;
    EXPECT(tsdb::TSDB::createDatabase(&db, "/tmp/__test.tsdb") == true);

    EXPECT(db->createSeries(1, tsdb::PageType::UINT64, "") == true);
    for (size_t i = 0; i < 100000; ++i) {
      EXPECT(db->insertUInt64(1, t0 + 20 * i, i) == true);
    }

    {
      tsdb::Cursor cursor(tsdb::PageType::UINT64);
      EXPECT(db->getCursor(1, &cursor) == true);

      for (size_t i = 0; i < 100000; ++i) {
        uint64_t ts;
        uint64_t value;
        EXPECT(cursor.next(&ts, &value) == true);
        EXPECT(ts == t0 + 20 * i);
        EXPECT(value == i);
      }
    }

    EXPECT(db->commit() == true);

    {
      tsdb::Cursor cursor(tsdb::PageType::UINT64);
      EXPECT(db->getCursor(1, &cursor) == true);

      for (size_t i = 0; i < 100000; ++i) {
        uint64_t ts;
        uint64_t value;
        EXPECT(cursor.next(&ts, &value) == true);
        EXPECT(ts == t0 + 20 * i);
        EXPECT(value == i);
      }
    }

    EXPECT(db->commit() == true);

    for (size_t i = 100000; i < 200000; ++i) {
      EXPECT(db->insertUInt64(1, t0 + 20 * i, i) == true);
    }

    EXPECT(db->commit() == true);

    {
      tsdb::Cursor cursor(tsdb::PageType::UINT64);
      EXPECT(db->getCursor(1, &cursor) == true);

      for (size_t i = 0; i < 200000; ++i) {
        uint64_t ts;
        uint64_t value;
        EXPECT(cursor.next(&ts, &value) == true);
        EXPECT(ts == t0 + 20 * i);
        EXPECT(value == i);
      }
    }
  }

  {
    std::unique_ptr<tsdb::TSDB> db;
    EXPECT(tsdb::TSDB::openDatabase(&db, "/tmp/__test.tsdb") == true);

    {
      tsdb::Cursor cursor(tsdb::PageType::UINT64);
      EXPECT(db->getCursor(1, &cursor) == true);

      for (size_t i = 0; i < 200000; ++i) {
        uint64_t ts;
        uint64_t value;
        EXPECT(cursor.next(&ts, &value) == true);
        EXPECT(ts == t0 + 20 * i);
        EXPECT(value == i);
      }
    }

    for (size_t i = 300000; i < 400000; ++i) {
      EXPECT(db->insertUInt64(1, t0 + 20 * i, i) == true);
    }

    EXPECT(db->commit() == true);

    {
      tsdb::Cursor cursor(tsdb::PageType::UINT64);
      EXPECT(db->getCursor(1, &cursor) == true);

      for (size_t i = 0; i < 200000; ++i) {
        uint64_t ts;
        uint64_t value;
        EXPECT(cursor.next(&ts, &value) == true);
        EXPECT(ts == t0 + 20 * i);
        EXPECT(value == i);
      }

      for (size_t i = 300000; i < 400000; ++i) {
        uint64_t ts;
        uint64_t value;
        EXPECT(cursor.next(&ts, &value) == true);
        EXPECT(ts == t0 + 20 * i);
        EXPECT(value == i);
      }
    }
  }

  {
    std::unique_ptr<tsdb::TSDB> db;
    EXPECT(tsdb::TSDB::openDatabase(&db, "/tmp/__test.tsdb") == true);

    for (size_t i = 200000; i < 300000; ++i) {
      EXPECT(db->insertUInt64(1, t0 + 20 * i, i) == true);
    }

    {
      tsdb::Cursor cursor(tsdb::PageType::UINT64);
      EXPECT(db->getCursor(1, &cursor) == true);

      for (size_t i = 0; i < 400000; ++i) {
        uint64_t ts;
        uint64_t value;
        EXPECT(cursor.next(&ts, &value) == true);
        EXPECT(ts == t0 + 20 * i);
        EXPECT(value == i);
      }
    }

    EXPECT(db->commit() == true);

    {
      tsdb::Cursor cursor(tsdb::PageType::UINT64);
      EXPECT(db->getCursor(1, &cursor) == true);

      for (size_t i = 0; i < 400000; ++i) {
        uint64_t ts;
        uint64_t value;
        EXPECT(cursor.next(&ts, &value) == true);
        EXPECT(ts == t0 + 20 * i);
        EXPECT(value == i);
      }
    }
  }

});


