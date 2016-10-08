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

using namespace fnordmetric::statsd;

UNIT_TEST(StatsdTest);

using LabelList = std::vector<std::pair<std::string, std::string>>;

TEST_CASE(StatsdTest, TestSimpleParseFromStatsdFormat, [] () {
  std::string key;
  std::string value;
  LabelList labels;

  std::string test_smpl = "/fnord/mymetric";
  auto cur = test_smpl.c_str();
  auto ret = parseStatsdSample(
      &cur,
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &value,
      &labels);

  EXPECT(ret == true);
  EXPECT_EQ(cur, test_smpl.c_str() + test_smpl.size());
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(labels.size(), 0);
  EXPECT_EQ(value, "");
});

TEST_CASE(StatsdTest, TestSimpleParseFromStatsdFormatWithValue, [] () {
  std::string key;
  std::string value;
  LabelList labels;

  std::string test_smpl = "/fnord/mymetric:34.23";
  auto cur = test_smpl.c_str();
  auto ret = parseStatsdSample(
      &cur,
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &value,
      &labels);

  EXPECT(ret == true);
  EXPECT_EQ(cur, test_smpl.c_str() + test_smpl.size());
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(labels.size(), 0);
  EXPECT_EQ(value, "34.23");
});

TEST_CASE(StatsdTest, TestParseFromStatsdFormatWithLabels, [] () {
  std::string key;
  std::string value;
  LabelList labels;

  std::string test_smpl = "/fnord/mymetric[label1=435][l2=str]:34.23";
  auto cur = test_smpl.c_str();
  auto ret = parseStatsdSample(
      &cur,
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &value,
      &labels);

  EXPECT(ret == true);
  EXPECT_EQ(cur, test_smpl.c_str() + test_smpl.size());
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(labels.size(), 2);
  EXPECT_EQ(labels[0].first, "label1");
  EXPECT_EQ(labels[0].second, "435");
  EXPECT_EQ(labels[1].first, "l2");
  EXPECT_EQ(labels[1].second, "str");
  EXPECT_EQ(value, "34.23");
});

TEST_CASE(StatsdTest, TestParseFromStatsdFormatWithMultipleSamples, [] () {
  std::string key;
  std::string value;
  LabelList labels;

  std::string test_smpl = "/fnord/mymetric:2.3\n/fnord/other:42.5\r\nfu:4.6";

  auto begin = test_smpl.c_str();
  auto cur = begin;
  auto end = test_smpl.c_str() + test_smpl.size();

  auto ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == true);
  EXPECT(cur == begin + 20);
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(labels.size(), 0);
  EXPECT_EQ(value, "2.3");

  ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == true);
  EXPECT(cur == begin + 39);
  EXPECT_EQ(key, "/fnord/other");
  EXPECT_EQ(labels.size(), 0);
  EXPECT_EQ(value, "42.5");

  ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == true);
  EXPECT(cur == end);
  EXPECT_EQ(key, "fu");
  EXPECT_EQ(labels.size(), 0);
  EXPECT_EQ(value, "4.6");
});

TEST_CASE(StatsdTest, TestParseFromStatsdFormatWithMultipleSmplLabels, [] () {
  std::string key;
  std::string value;
  LabelList labels;

  std::string test_smpl = "/fmet[l1=l]:2.3\noth[l3=x]:42.5\r\nfu[a=b]:4.6\r\n";

  auto begin = test_smpl.c_str();
  auto cur = begin;
  auto end = test_smpl.c_str() + test_smpl.size();

  auto ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == true);
  EXPECT(cur == begin + 16);
  EXPECT_EQ(key, "/fmet");
  EXPECT_EQ(labels.size(), 1);
  EXPECT_EQ(value, "2.3");

  labels.clear();
  ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == true);
  EXPECT(cur == begin + 32);
  EXPECT_EQ(key, "oth");
  EXPECT_EQ(labels.size(), 1);
  EXPECT_EQ(value, "42.5");

  labels.clear();
  ret = parseStatsdSample(
      &cur,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == true);
  EXPECT(cur == end);
  EXPECT_EQ(key, "fu");
  EXPECT_EQ(labels.size(), 1);
  EXPECT_EQ(value, "4.6");
});

