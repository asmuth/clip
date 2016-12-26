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
    LabelledSample smpl(Sample(1482776368000000, 10), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776369000000, 5), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
  }

  {
    LabelledSample smpl(Sample(1482776370000000, 8), {});
    auto rc = service->insertSample("users_online", smpl);
    EXPECT(rc.isSuccess());
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
    EXPECT(cursor.next(&ts, &val) == false);
  }
});

