/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include <metricdb/statsd.h>
#include <util/unittest.h>

using namespace utilmetric::metricdb;

UNIT_TEST(StatsdTest);

using LabelList = std::vector<std::pair<std::string, std::string>>;

TEST_CASE(StatsdTest, TestSimpleParseFromStatsdFormat, [] () {
  std::string key;
  std::string value;
  LabelList labels;

  std::string test_smpl = "/fnord/mymetric";
  auto ret = StatsdServer::parseStatsdSample(
      test_smpl.c_str(),
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &value,
      &labels);

  EXPECT_EQ(ret, test_smpl.c_str() + test_smpl.size());
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(labels.size(), 0);
  EXPECT_EQ(value, "");
});

TEST_CASE(StatsdTest, TestSimpleParseFromStatsdFormatWithValue, [] () {
  std::string key;
  std::string value;
  LabelList labels;

  std::string test_smpl = "/fnord/mymetric:34.23";
  auto ret = StatsdServer::parseStatsdSample(
      test_smpl.c_str(),
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &value,
      &labels);

  EXPECT_EQ(ret, test_smpl.c_str() + test_smpl.size());
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(labels.size(), 0);
  EXPECT_EQ(value, "34.23");
});

TEST_CASE(StatsdTest, TestParseFromStatsdFormatWithLabels, [] () {
  std::string key;
  std::string value;
  LabelList labels;

  std::string test_smpl = "/fnord/mymetric[label1=435][l2=str]:34.23";
  auto ret = StatsdServer::parseStatsdSample(
      test_smpl.c_str(),
      test_smpl.c_str() + test_smpl.size(),
      &key,
      &value,
      &labels);

  EXPECT_EQ(ret, test_smpl.c_str() + test_smpl.size());
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
  auto end = test_smpl.c_str() + test_smpl.size();

  auto ret = StatsdServer::parseStatsdSample(
      begin,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == begin + 20);
  EXPECT_EQ(key, "/fnord/mymetric");
  EXPECT_EQ(labels.size(), 0);
  EXPECT_EQ(value, "2.3");

  ret = StatsdServer::parseStatsdSample(
      ret,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == begin + 39);
  EXPECT_EQ(key, "/fnord/other");
  EXPECT_EQ(labels.size(), 0);
  EXPECT_EQ(value, "42.5");

  ret = StatsdServer::parseStatsdSample(
      ret,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == end);
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
  auto end = test_smpl.c_str() + test_smpl.size();

  auto ret = StatsdServer::parseStatsdSample(
      begin,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == begin + 16);
  EXPECT_EQ(key, "/fmet");
  EXPECT_EQ(labels.size(), 1);
  EXPECT_EQ(value, "2.3");

  labels.clear();
  ret = StatsdServer::parseStatsdSample(
      ret,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == begin + 32);
  EXPECT_EQ(key, "oth");
  EXPECT_EQ(labels.size(), 1);
  EXPECT_EQ(value, "42.5");

  labels.clear();
  ret = StatsdServer::parseStatsdSample(
      ret,
      end,
      &key,
      &value,
      &labels);

  EXPECT(ret == end);
  EXPECT_EQ(key, "fu");
  EXPECT_EQ(labels.size(), 1);
  EXPECT_EQ(value, "4.6");
});
