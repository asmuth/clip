/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/metricdb/metrickey.h>
#include <fnordmetric/util/unittest.h>

using namespace fnordmetric::metricdb;

UNIT_TEST(MetricKeyTest);

TEST_CASE(MetricKeyTest, TestSimpleParseFromStatsdFormat, [] () {
  MetricKey mkey;
  std::string value;

  MetricKey::parseStatsdFormat(
      "/fnord/mymetric",
      &mkey,
      &value);

  EXPECT_EQ(mkey.key(), "/fnord/mymetric");
  EXPECT_EQ(mkey.labels().size(), 0);
  EXPECT_EQ(value, "")
});

TEST_CASE(MetricKeyTest, TestSimpleParseFromStatsdFormatWithValue, [] () {
  MetricKey mkey;
  std::string value;

  MetricKey::parseStatsdFormat(
      "/fnord/mymetric:34.23",
      &mkey,
      &value);

  EXPECT_EQ(mkey.key(), "/fnord/mymetric");
  EXPECT_EQ(mkey.labels().size(), 0);
  EXPECT_EQ(value, "34.23")
});

TEST_CASE(MetricKeyTest, TestParseFromStatsdFormatWithLabels, [] () {
  MetricKey mkey;
  std::string value;

  MetricKey::parseStatsdFormat(
      "/fnord/mymetric[label1=435][l2=str]:34.23",
      &mkey,
      &value);

  EXPECT_EQ(mkey.key(), "/fnord/mymetric");
  EXPECT_EQ(mkey.labels().size(), 2);
  EXPECT_EQ(mkey.labels()[0].first, "label1");
  EXPECT_EQ(mkey.labels()[0].second, "435");
  EXPECT_EQ(mkey.labels()[1].first, "l2");
  EXPECT_EQ(mkey.labels()[1].second, "str");
  EXPECT_EQ(value, "34.23")
});

