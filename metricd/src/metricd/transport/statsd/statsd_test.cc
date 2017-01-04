/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <metricd/util/unittest.h>
#include <metricd/transport/statsd/statsd.h>

using namespace fnordmetric;
using namespace fnordmetric::statsd;

UNIT_TEST(StatsdTest);

TEST_CASE(StatsdTest, TestSimpleParseFromStatsdFormat, [] () {
  std::string key;
  std::string series_id;
  std::string value;

  std::string test_smpl = "/fnord/mymetric";
  auto cur = test_smpl.c_str();
  auto ret = parseStatsdSample(
      &cur,
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &series_id,
      &value);

  EXPECT(ret == true);
  EXPECT_EQ(cur, test_smpl.c_str() + test_smpl.size());
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(series_id, "");
  EXPECT_EQ(value, "");
});

TEST_CASE(StatsdTest, TestSimpleParseFromStatsdFormatWithValue, [] () {
  std::string key;
  std::string series_id;
  std::string value;

  std::string test_smpl = "/fnord/mymetric:34.23";
  auto cur = test_smpl.c_str();
  auto ret = parseStatsdSample(
      &cur,
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &series_id,
      &value);

  EXPECT(ret == true);
  EXPECT_EQ(cur, test_smpl.c_str() + test_smpl.size());
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(series_id, "");
  EXPECT_EQ(value, "34.23");
});

TEST_CASE(StatsdTest, TestParseFromStatsdFormatWithSeriesID, [] () {
  std::string key;
  std::string series_id;
  std::string value;

  std::string test_smpl = "/fnord/mymetric[myfancyseriesid]:34.23";
  auto cur = test_smpl.c_str();
  auto ret = parseStatsdSample(
      &cur,
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &series_id,
      &value);

  EXPECT(ret == true);
  EXPECT_EQ(cur, test_smpl.c_str() + test_smpl.size());
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(series_id, "myfancyseriesid");
  EXPECT_EQ(value, "34.23");
});

TEST_CASE(StatsdTest, TestParseFromStatsdFormatWithMultipleSamples, [] () {
  std::string key;
  std::string series_id;
  std::string value;

  std::string test_smpl = "/fnord/mymetric:2.3\n/fnord/other:42.5\r\nfu:4.6";

  auto begin = test_smpl.c_str();
  auto cur = begin;
  auto end = test_smpl.c_str() + test_smpl.size();

  auto ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &series_id,
      &value);

  EXPECT(ret == true);
  EXPECT(cur == begin + 20);
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(value, "2.3");

  ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &series_id,
      &value);

  EXPECT(ret == true);
  EXPECT(cur == begin + 39);
  EXPECT_EQ(key, "/fnord/other");
  EXPECT_EQ(series_id, "");
  EXPECT_EQ(value, "42.5");

  ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &series_id,
      &value);

  EXPECT(ret == true);
  EXPECT(cur == end);
  EXPECT_EQ(key, "fu");
  EXPECT_EQ(series_id, "");
  EXPECT_EQ(value, "4.6");
});

TEST_CASE(StatsdTest, TestParseFromStatsdFormatWithMultipleSmplLabels, [] () {
  std::string key;
  std::string series_id;
  std::string value;

  std::string test_smpl = "/fmet[l1]:2.3\noth[l3]:42.5\r\nfu[xx]:4.6\r\n";

  auto begin = test_smpl.c_str();
  auto cur = begin;
  auto end = test_smpl.c_str() + test_smpl.size();

  auto ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &series_id,
      &value);

  EXPECT(ret == true);
  EXPECT_EQ(key, "/fmet");
  EXPECT_EQ(series_id, "l1");
  EXPECT_EQ(value, "2.3");

  ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &series_id,
      &value);

  EXPECT(ret == true);
  EXPECT_EQ(key, "oth");
  EXPECT_EQ(series_id, "l3");
  EXPECT_EQ(value, "42.5");

  ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &series_id,
      &value);

  EXPECT(ret == true);
  EXPECT(cur == end);
  EXPECT_EQ(key, "fu");
  EXPECT_EQ(series_id, "xx");
  EXPECT_EQ(value, "4.6");
});

