/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/io/fileutil.h>
#include <fnordmetric/metricdb/metric.h>
#include <fnordmetric/util/unittest.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace fnordmetric::metricdb;
using namespace fnord::io;

UNIT_TEST(MetricTest);

const char kTestRepoPath[] = "/tmp/__fnordmetric_test_metricrepo2";

TEST_CASE(MetricTest, TestCreateNewMetric, [] () {
  io::FileUtil::mkdir_p(kTestRepoPath);
  FileRepository file_repo(kTestRepoPath);

  Metric metric("mymetric", &file_repo);

  Sample<double> sample;
  sample.key = "mymetric";
  sample.value = 23.5f;
  sample.labels.emplace_back("mylabel", "myvalue");
  metric.addSample(sample);
});



