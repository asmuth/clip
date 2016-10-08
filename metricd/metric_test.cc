/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
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
#include "metricd/metric.h"
#include "metricd/metric_map.h"

using namespace fnordmetric;

UNIT_TEST(MetricTest);

TEST_CASE(MetricTest, TestMetricSeriesList, [] () {
  SeriesIDProvider id_provider;
  MetricSeriesList series_list;
  EXPECT(series_list.getSize() == 0);
  std::shared_ptr<MetricSeries> series1;
  EXPECT(
      series_list.findOrCreateSeries(
          &id_provider,
          {{ "hostname", "r1s1"}, { "datacenter", "ams1"}},
          &series1).isSuccess());
  EXPECT(series_list.getSize() == 1);
  std::shared_ptr<MetricSeries> series2;
  EXPECT(
      series_list.findOrCreateSeries(
          &id_provider,
          {{ "hostname", "r1s1"}, { "datacenter", "ams1"}},
          &series2).isSuccess());
  EXPECT(series_list.getSize() == 1);
  EXPECT(series1.get() == series2.get());
  std::shared_ptr<MetricSeries> series3;
  EXPECT(
      series_list.findOrCreateSeries(
          &id_provider,
          {{ "hostname", "r1s2"}, { "datacenter", "ams1"}},
          &series2).isSuccess());
  EXPECT(series_list.getSize() == 2);
  EXPECT(series2.get() != series3.get());
});

