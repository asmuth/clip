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
#include "metricd/util/exception.h"
#include "metricd/util/unittest.h"
#include "metricd/util/time.h"
#include "metricd/metric_service.h"
#include "metricd/sample.h"

using namespace fnordmetric;

UNIT_TEST(MetricServiceTest);

TEST_CASE(MetricServiceTest, TestSumAggregator, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.data_type = MetricDataType::UINT64;
    mc.aggregation = MetricAggregationType::SUM;
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
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val) == false);
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 15), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776410000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776368000000, 10), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }
  {
    LabelledSample smpl(Sample(1482776370000000, 8), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776369000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776590000000, 42), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776788000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776340000000);
    EXPECT(val == 23);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(val == 17);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776460000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776520000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776580000000);
    EXPECT(val == 42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776640000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776700000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776760000000);
    EXPECT(val == 5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776820000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

TEST_CASE(MetricServiceTest, TestSumAggregatorWithDownsampling, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.data_type = MetricDataType::UINT64;
    mc.aggregation = MetricAggregationType::SUM;
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
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val) == false);
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 15), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776410000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776368000000, 10), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }
  {
    LabelledSample smpl(Sample(1482776370000000, 8), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776369000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776590000000, 42), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776788000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776100000000);
    EXPECT(val == 23);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(val == 59);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776700000000);
    EXPECT(val == 5);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

TEST_CASE(MetricServiceTest, TestSumAggregatorWithUpsampling, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.data_type = MetricDataType::UINT64;
    mc.aggregation = MetricAggregationType::SUM;
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
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val) == false);
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 15), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776410000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776368000000, 10), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }
  {
    LabelledSample smpl(Sample(1482776370000000, 8), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776369000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776590000000, 42), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776788000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776340000000);
    EXPECT(val == 8);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776350000000);
    EXPECT(val == 3);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776360000000);
    EXPECT(val == 3);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776370000000);
    EXPECT(val == 3);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776380000000);
    EXPECT(val == 3);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776390000000);
    EXPECT(val == 3);

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(val == 7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776410000000);
    EXPECT(val == 2);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776420000000);
    EXPECT(val == 2);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776430000000);
    EXPECT(val == 2);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776440000000);
    EXPECT(val == 2);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776450000000);
    EXPECT(val == 2);

    for (uint64_t expected_ts = 1482776460000000, i = 0; i < 12; ++i) {
      EXPECT(cursor.next(&ts, &val));
      EXPECT(ts == expected_ts);
      EXPECT(val == 0);
      expected_ts += 10000000;
    }

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776580000000);
    EXPECT(val == 7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776590000000);
    EXPECT(val == 7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776600000000);
    EXPECT(val == 7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776610000000);
    EXPECT(val == 7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776620000000);
    EXPECT(val == 7);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776630000000);
    EXPECT(val == 7);

    for (uint64_t expected_ts = 1482776640000000, i = 0; i < 12; ++i) {
      EXPECT(cursor.next(&ts, &val));
      EXPECT(ts == expected_ts);
      EXPECT(val == 0);
      expected_ts += 10000000;
    }

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776760000000);
    EXPECT(val == 5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776770000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776780000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776790000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776800000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776810000000);
    EXPECT(val == 0);

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776820000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

TEST_CASE(MetricServiceTest, TestMaxAggregator, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.data_type = MetricDataType::UINT64;
    mc.aggregation = MetricAggregationType::MAX;
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
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val) == false);
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 15), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776410000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776368000000, 10), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }
  {
    LabelledSample smpl(Sample(1482776370000000, 8), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776369000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776590000000, 42), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776788000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776340000000);
    EXPECT(val == 10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(val == 15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776460000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776520000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776580000000);
    EXPECT(val == 42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776640000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776700000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776760000000);
    EXPECT(val == 5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776820000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

TEST_CASE(MetricServiceTest, TestMaxAggregatorWithDownsampling, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.data_type = MetricDataType::UINT64;
    mc.aggregation = MetricAggregationType::MAX;
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
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val) == false);
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 15), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776410000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776368000000, 10), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }
  {
    LabelledSample smpl(Sample(1482776370000000, 8), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776369000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776590000000, 42), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776788000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776100000000);
    EXPECT(val == 10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(val == 42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776700000000);
    EXPECT(val == 5);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

TEST_CASE(MetricServiceTest, TestMaxAggregatorWithUpsampling, [] () {
  mkdir("/tmp/__test_metricd", 0755);
  unlink("/tmp/__test_metricd/default.tsdb");

  ConfigList config;

  {
    MetricConfig mc;
    mc.is_valid = true;
    mc.data_type = MetricDataType::UINT64;
    mc.aggregation = MetricAggregationType::MAX;
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
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val) == false);
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 15), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776400000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776410000000, 1), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776368000000, 10), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }
  {
    LabelledSample smpl(Sample(1482776370000000, 8), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776369000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776590000000, 42), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776788000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    auto cursor = service->getCursor("users_online", 1); // FIXME hack
    uint64_t ts;
    uint64_t val;
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776340000000);
    EXPECT(val == 10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776350000000);
    EXPECT(val == 10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776360000000);
    EXPECT(val == 10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776370000000);
    EXPECT(val == 10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776380000000);
    EXPECT(val == 10);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776390000000);
    EXPECT(val == 10);

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776400000000);
    EXPECT(val == 15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776410000000);
    EXPECT(val == 15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776420000000);
    EXPECT(val == 15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776430000000);
    EXPECT(val == 15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776440000000);
    EXPECT(val == 15);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776450000000);
    EXPECT(val == 15);

    for (uint64_t expected_ts = 1482776460000000, i = 0; i < 12; ++i) {
      EXPECT(cursor.next(&ts, &val));
      EXPECT(ts == expected_ts);
      EXPECT(val == 0);
      expected_ts += 10000000;
    }

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776580000000);
    EXPECT(val == 42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776590000000);
    EXPECT(val == 42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776600000000);
    EXPECT(val == 42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776610000000);
    EXPECT(val == 42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776620000000);
    EXPECT(val == 42);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776630000000);
    EXPECT(val == 42);

    for (uint64_t expected_ts = 1482776640000000, i = 0; i < 12; ++i) {
      EXPECT(cursor.next(&ts, &val));
      EXPECT(ts == expected_ts);
      EXPECT(val == 0);
      expected_ts += 10000000;
    }

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776760000000);
    EXPECT(val == 5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776770000000);
    EXPECT(val == 5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776780000000);
    EXPECT(val == 5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776790000000);
    EXPECT(val == 5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776800000000);
    EXPECT(val == 5);
    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776810000000);
    EXPECT(val == 5);

    EXPECT(cursor.next(&ts, &val));
    EXPECT(ts == 1482776820000000);
    EXPECT(val == 0);
    EXPECT(cursor.next(&ts, &val) == false);
  }
});
