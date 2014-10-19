/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/statsd.h>
#include <fnordmetric/util/unittest.h>

using namespace fnordmetric::metricdb;

UNIT_TEST(StatsdTest);

TEST_CASE(StatsdTest, TestSimpleParseFromStatsdFormat, [] () {
  Sample<std::string> sample;

  parseStatsdSample(
      "/fnord/mymetric",
      &sample);

  EXPECT_EQ(sample.key, "/fnord/mymetric");
  EXPECT_EQ(sample.labels.size(), 0);
  EXPECT_EQ(sample.value, "")
});

TEST_CASE(StatsdTest, TestSimpleParseFromStatsdFormatWithValue, [] () {
  Sample<std::string> sample;

  parseStatsdSample(
      "/fnord/mymetric:34.23",
      &sample);

  EXPECT_EQ(sample.key, "/fnord/mymetric");
  EXPECT_EQ(sample.labels.size(), 0);
  EXPECT_EQ(sample.value, "34.23")
});

TEST_CASE(StatsdTest, TestParseFromStatsdFormatWithLabels, [] () {
  Sample<std::string> sample;

  parseStatsdSample(
      "/fnord/mymetric[label1=435][l2=str]:34.23",
      &sample);

  EXPECT_EQ(sample.key, "/fnord/mymetric");
  EXPECT_EQ(sample.labels.size(), 2);
  EXPECT_EQ(sample.labels[0].first, "label1");
  EXPECT_EQ(sample.labels[0].second, "435");
  EXPECT_EQ(sample.labels[1].first, "l2");
  EXPECT_EQ(sample.labels[1].second, "str");
  EXPECT_EQ(sample.value, "34.23")
});

