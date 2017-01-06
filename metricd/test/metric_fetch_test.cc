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
#include <sys/stat.h>
#include <sys/types.h>
#include "unittest.h"
#include "metricd/util/time.h"
#include "metricd/metric_service.h"
#include "metricd/sample.h"

using namespace fnordmetric;

UNIT_TEST(MetricServiceTest);

TEST_CASE(MetricServiceTest, TestCounterMetric, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.kind = MetricKind::COUNTER_UINT64;
    mc.granularity = 60 * kMicrosPerSecond;
    config.addMetricConfig("users_online", mc);
  }

  std::unique_ptr<MetricService> service;
  auto start_rc = MetricService::startService(
      "/tmp/__test_metricd",
      &config,
      &service);

  EXPECT(start_rc.isSuccess());

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "15");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "1");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776410000000,
        "1");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776368000000,
        "10");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776370000000,
        "8");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776369000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776590000000,
        "42");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776788000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    MetricCursorOptions cursor_opts;
    cursor_opts.series_name = SeriesNameType("");
    cursor_opts.time_begin = 1482776220000000;
    cursor_opts.time_limit = 1482776900000000;

    MetricCursor cursor;
    auto cursor_rc = service->fetchData("users_online", cursor_opts, &cursor);
    EXPECT(cursor_rc.isSuccess());

    uint64_t ts;
    tval_ref val;
    val.len = sizeof(uint64_t);
    val.data = alloca(val.len);
    val.type = tval_type::UINT64;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776220000000);
    EXPECT(*((uint64_t*) val.data) == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776280000000);
    EXPECT(*((uint64_t*) val.data) == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776340000000);
    EXPECT(*((uint64_t*) val.data) == 23);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(*((uint64_t*) val.data) ==17);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776460000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776520000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776580000000);
    EXPECT(*((uint64_t*) val.data) ==42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776640000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776700000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776760000000);
    EXPECT(*((uint64_t*) val.data) ==5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776820000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776880000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776940000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

TEST_CASE(MetricServiceTest, TestCounterMetricWithDownsampling, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.kind = MetricKind::COUNTER_UINT64;
    mc.granularity = 60 * kMicrosPerSecond;
    mc.display_granularity = 300 * kMicrosPerSecond;
    config.addMetricConfig("users_online", mc);
  }

  std::unique_ptr<MetricService> service;
  auto start_rc = MetricService::startService(
      "/tmp/__test_metricd",
      &config,
      &service);

  EXPECT(start_rc.isSuccess());

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "15");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "1");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776410000000,
        "1");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776368000000,
        "10");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776370000000,
        "8");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776369000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776590000000,
        "42");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776788000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    MetricCursorOptions cursor_opts;
    cursor_opts.series_name = SeriesNameType("");
    cursor_opts.time_begin = 1482776100000000;
    cursor_opts.time_limit = 1482776700000000;

    MetricCursor cursor;
    auto cursor_rc = service->fetchData("users_online", cursor_opts, &cursor);
    EXPECT(cursor_rc.isSuccess());

    uint64_t ts;
    tval_ref val;
    val.len = sizeof(uint64_t);
    val.data = alloca(val.len);
    val.type = tval_type::UINT64;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776100000000);
    EXPECT(*((uint64_t*) val.data) ==23);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(*((uint64_t*) val.data) ==59);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776700000000);
    EXPECT(*((uint64_t*) val.data) ==5);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

TEST_CASE(MetricServiceTest, TestCounterMetricWithUpsampling, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.kind = MetricKind::COUNTER_UINT64;
    mc.granularity = 60 * kMicrosPerSecond;
    mc.display_granularity = 10 * kMicrosPerSecond;
    config.addMetricConfig("users_online", mc);
  }

  std::unique_ptr<MetricService> service;
  auto start_rc = MetricService::startService(
      "/tmp/__test_metricd",
      &config,
      &service);

  EXPECT(start_rc.isSuccess());

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "15");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "1");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776410000000,
        "1");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776368000000,
        "10");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776370000000,
        "8");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776369000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776590000000,
        "42");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776788000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    MetricCursorOptions cursor_opts;
    cursor_opts.series_name = SeriesNameType("");
    cursor_opts.time_begin = 1482776340000000;
    cursor_opts.time_limit = 1482776820000000;

    MetricCursor cursor;
    auto cursor_rc = service->fetchData("users_online", cursor_opts, &cursor);
    EXPECT(cursor_rc.isSuccess());

    uint64_t ts;
    tval_ref val;
    val.len = sizeof(uint64_t);
    val.data = alloca(val.len);
    val.type = tval_type::UINT64;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776340000000);
    EXPECT(*((uint64_t*) val.data) ==8);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776350000000);
    EXPECT(*((uint64_t*) val.data) ==3);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776360000000);
    EXPECT(*((uint64_t*) val.data) ==3);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776370000000);
    EXPECT(*((uint64_t*) val.data) ==3);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776380000000);
    EXPECT(*((uint64_t*) val.data) ==3);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776390000000);
    EXPECT(*((uint64_t*) val.data) ==3);

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(*((uint64_t*) val.data) ==7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776410000000);
    EXPECT(*((uint64_t*) val.data) ==2);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776420000000);
    EXPECT(*((uint64_t*) val.data) ==2);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776430000000);
    EXPECT(*((uint64_t*) val.data) ==2);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776440000000);
    EXPECT(*((uint64_t*) val.data) ==2);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776450000000);
    EXPECT(*((uint64_t*) val.data) ==2);

    for (uint64_t expected_ts = 1482776460000000, i = 0; i < 12; ++i) {
      EXPECT(cursor.next(&ts, &val));
      EXPECT(ts == expected_ts);
      EXPECT(*((uint64_t*) val.data) ==0);
      expected_ts += 10000000;
    }

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776580000000);
    EXPECT(*((uint64_t*) val.data) ==7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776590000000);
    EXPECT(*((uint64_t*) val.data) ==7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776600000000);
    EXPECT(*((uint64_t*) val.data) ==7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776610000000);
    EXPECT(*((uint64_t*) val.data) ==7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776620000000);
    EXPECT(*((uint64_t*) val.data) ==7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776630000000);
    EXPECT(*((uint64_t*) val.data) ==7);

    for (uint64_t expected_ts = 1482776640000000, i = 0; i < 12; ++i) {
      EXPECT(cursor.next(&ts, &val));
      EXPECT(ts == expected_ts);
      EXPECT(*((uint64_t*) val.data) ==0);
      expected_ts += 10000000;
    }

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776760000000);
    EXPECT(*((uint64_t*) val.data) ==5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776770000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776780000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776790000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776800000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776810000000);
    EXPECT(*((uint64_t*) val.data) ==0);

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776820000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

TEST_CASE(MetricServiceTest, TestCounterMetricWithSummary, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.kind = MetricKind::COUNTER_UINT64;
    mc.granularity = 60 * kMicrosPerSecond;
    config.addMetricConfig("users_online", mc);
  }

  std::unique_ptr<MetricService> service;
  auto start_rc = MetricService::startService(
      "/tmp/__test_metricd",
      &config,
      &service);

  EXPECT(start_rc.isSuccess());

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType("one"),
        1482776220000000,
        "10");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType("one"),
        1482776280000000,
        "10");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType("one"),
        1482776340000000,
        "10");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType("one"),
        1482776400000000,
        "10");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType("one"),
        1482776460000000,
        "10");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType("two"),
        1482776230000000,
        "13");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType("two"),
        1482776290000000,
        "13");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType("two"),
        1482776350000000,
        "13");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType("two"),
        1482776410000000,
        "13");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType("two"),
        1482776470000000,
        "13");
    EXPECT(rc.isSuccess());
  }

  {
    MetricCursorOptions cursor_opts;
    cursor_opts.cursor_type = MetricCursorType::SUMMARY;
    cursor_opts.time_begin = 1482776220000000;
    cursor_opts.time_limit = 1482776460000000;

    MetricCursor cursor;
    auto cursor_rc = service->fetchData("users_online", cursor_opts, &cursor);
    EXPECT(cursor_rc.isSuccess());

    uint64_t ts;
    tval_ref val;
    val.len = sizeof(uint64_t);
    val.data = alloca(val.len);
    val.type = tval_type::UINT64;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776220000000);
    EXPECT_EQ(*((uint64_t*) val.data), 23);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776280000000);
    EXPECT(*((uint64_t*) val.data) == 23);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776340000000);
    EXPECT(*((uint64_t*) val.data) == 23);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(*((uint64_t*) val.data) == 23);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776460000000);
    EXPECT(*((uint64_t*) val.data) == 23);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

TEST_CASE(MetricServiceTest, TestMaxMetric, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.kind = MetricKind::MAX_UINT64;
    mc.granularity = 60 * kMicrosPerSecond;
    config.addMetricConfig("users_online", mc);
  }

  std::unique_ptr<MetricService> service;
  auto start_rc = MetricService::startService(
      "/tmp/__test_metricd",
      &config,
      &service);

  EXPECT(start_rc.isSuccess());

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "15");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "1");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776410000000,
        "1");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776368000000,
        "10");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776370000000,
        "8");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776369000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776590000000,
        "42");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776788000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    MetricCursorOptions cursor_opts;
    cursor_opts.series_name = SeriesNameType("");
    cursor_opts.time_begin = 1482776220000000;
    cursor_opts.time_limit = 1482776900000000;

    MetricCursor cursor;
    auto cursor_rc = service->fetchData("users_online", cursor_opts, &cursor);
    EXPECT(cursor_rc.isSuccess());

    uint64_t ts;
    tval_ref val;
    val.len = sizeof(uint64_t);
    val.data = alloca(val.len);
    val.type = tval_type::UINT64;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776220000000);
    EXPECT(*((uint64_t*) val.data) == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776280000000);
    EXPECT(*((uint64_t*) val.data) == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776340000000);
    EXPECT(*((uint64_t*) val.data) ==10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(*((uint64_t*) val.data) ==15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776460000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776520000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776580000000);
    EXPECT(*((uint64_t*) val.data) ==42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776640000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776700000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776760000000);
    EXPECT(*((uint64_t*) val.data) ==5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776820000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776880000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776940000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

TEST_CASE(MetricServiceTest, TestMaxMetricWithDownsampling, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.kind = MetricKind::MAX_UINT64;
    mc.granularity = 60 * kMicrosPerSecond;
    mc.display_granularity = 300 * kMicrosPerSecond;
    config.addMetricConfig("users_online", mc);
  }

  std::unique_ptr<MetricService> service;
  auto start_rc = MetricService::startService(
      "/tmp/__test_metricd",
      &config,
      &service);

  EXPECT(start_rc.isSuccess());

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "15");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "1");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776410000000,
        "1");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776368000000,
        "10");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776370000000,
        "8");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776369000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776590000000,
        "42");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776788000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    MetricCursorOptions cursor_opts;
    cursor_opts.series_name = SeriesNameType("");
    cursor_opts.time_begin = 1482776100000000;
    cursor_opts.time_limit = 1482776700000000;

    MetricCursor cursor;
    auto cursor_rc = service->fetchData("users_online", cursor_opts, &cursor);
    EXPECT(cursor_rc.isSuccess());

    uint64_t ts;
    tval_ref val;
    val.len = sizeof(uint64_t);
    val.data = alloca(val.len);
    val.type = tval_type::UINT64;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776100000000);
    EXPECT(*((uint64_t*) val.data) ==10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(*((uint64_t*) val.data) ==42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776700000000);
    EXPECT(*((uint64_t*) val.data) ==5);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

TEST_CASE(MetricServiceTest, TestMaxMetricWithUpsampling, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.kind = MetricKind::MAX_UINT64;
    mc.granularity = 60 * kMicrosPerSecond;
    mc.display_granularity = 10 * kMicrosPerSecond;
    config.addMetricConfig("users_online", mc);
  }

  std::unique_ptr<MetricService> service;
  auto start_rc = MetricService::startService(
      "/tmp/__test_metricd",
      &config,
      &service);

  EXPECT(start_rc.isSuccess());

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "15");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776400000000,
        "1");
    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776410000000,
        "1");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776368000000,
        "10");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776370000000,
        "8");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776369000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776590000000,
        "42");

    EXPECT(rc.isSuccess());
  }

  {
    auto rc = service->insertSample(
        "users_online",
        SeriesNameType(""),
        1482776788000000,
        "5");

    EXPECT(rc.isSuccess());
  }

  {
    MetricCursorOptions cursor_opts;
    cursor_opts.series_name = SeriesNameType("");
    cursor_opts.time_begin = 1482776340000000;
    cursor_opts.time_limit = 1482776820000000;

    MetricCursor cursor;
    auto cursor_rc = service->fetchData("users_online", cursor_opts, &cursor);
    EXPECT(cursor_rc.isSuccess());

    uint64_t ts;
    tval_ref val;
    val.len = sizeof(uint64_t);
    val.data = alloca(val.len);
    val.type = tval_type::UINT64;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776340000000);
    EXPECT(*((uint64_t*) val.data) ==10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776350000000);
    EXPECT(*((uint64_t*) val.data) ==10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776360000000);
    EXPECT(*((uint64_t*) val.data) ==10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776370000000);
    EXPECT(*((uint64_t*) val.data) ==10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776380000000);
    EXPECT(*((uint64_t*) val.data) ==10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776390000000);
    EXPECT(*((uint64_t*) val.data) ==10);

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(*((uint64_t*) val.data) ==15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776410000000);
    EXPECT(*((uint64_t*) val.data) ==15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776420000000);
    EXPECT(*((uint64_t*) val.data) ==15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776430000000);
    EXPECT(*((uint64_t*) val.data) ==15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776440000000);
    EXPECT(*((uint64_t*) val.data) ==15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776450000000);
    EXPECT(*((uint64_t*) val.data) ==15);

    for (uint64_t expected_ts = 1482776460000000, i = 0; i < 12; ++i) {
      EXPECT(cursor.next(&ts, &val));
      EXPECT(ts == expected_ts);
      EXPECT(*((uint64_t*) val.data) ==0);
      expected_ts += 10000000;
    }

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776580000000);
    EXPECT(*((uint64_t*) val.data) ==42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776590000000);
    EXPECT(*((uint64_t*) val.data) ==42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776600000000);
    EXPECT(*((uint64_t*) val.data) ==42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776610000000);
    EXPECT(*((uint64_t*) val.data) ==42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776620000000);
    EXPECT(*((uint64_t*) val.data) ==42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776630000000);
    EXPECT(*((uint64_t*) val.data) ==42);

    for (uint64_t expected_ts = 1482776640000000, i = 0; i < 12; ++i) {
      EXPECT(cursor.next(&ts, &val));
      EXPECT(ts == expected_ts);
      EXPECT(*((uint64_t*) val.data) ==0);
      expected_ts += 10000000;
    }

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776760000000);
    EXPECT(*((uint64_t*) val.data) ==5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776770000000);
    EXPECT(*((uint64_t*) val.data) ==5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776780000000);
    EXPECT(*((uint64_t*) val.data) ==5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776790000000);
    EXPECT(*((uint64_t*) val.data) ==5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776800000000);
    EXPECT(*((uint64_t*) val.data) ==5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776810000000);
    EXPECT(*((uint64_t*) val.data) ==5);

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776820000000);
    EXPECT(*((uint64_t*) val.data) ==0);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});
