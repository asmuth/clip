/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include "fnord/base/stringutil.h"
#include "fnord/test/unittest.h"

using namespace fnord;

UNIT_TEST(StringUtilTest);

TEST_CASE(StringUtilTest, TestToString, [] () {
  EXPECT_EQ(StringUtil::toString(123), "123");
  EXPECT_EQ(StringUtil::toString(1230000000), "1230000000");
  EXPECT_EQ(StringUtil::toString(24.5), "24.5");
  EXPECT_EQ(StringUtil::toString("abc"), "abc");
  EXPECT_EQ(StringUtil::toString(std::string("abc")), "abc");
});

TEST_CASE(StringUtilTest, TestStripTrailingSlashes, [] () {
  std::string s1 = "fnord/bar/";
  StringUtil::stripTrailingSlashes(&s1);
  EXPECT_EQ(s1, "fnord/bar");

  std::string s2 = "fnord/bar///";
  StringUtil::stripTrailingSlashes(&s2);
  EXPECT_EQ(s2, "fnord/bar");

  std::string s3 = "fnord/bar";
  StringUtil::stripTrailingSlashes(&s3);
  EXPECT_EQ(s3, "fnord/bar");

  std::string s4 = "/";
  StringUtil::stripTrailingSlashes(&s4);
  EXPECT_EQ(s4, "");
});

TEST_CASE(StringUtilTest, TestBeginsWith, [] () {
  EXPECT_TRUE(StringUtil::beginsWith("fnord", "fn"));
  EXPECT_TRUE(StringUtil::beginsWith("fnahrad", "fn"));
  EXPECT_FALSE(StringUtil::beginsWith("ford", "fn"));
  EXPECT_TRUE(StringUtil::beginsWith("fnord", "fnord"));
  EXPECT_FALSE(StringUtil::beginsWith("fnord", "fnordbar"));
});

TEST_CASE(StringUtilTest, TestEndsWith, [] () {
  EXPECT_TRUE(StringUtil::endsWith("fnord", "ord"));
  EXPECT_TRUE(StringUtil::endsWith("ford", "ord"));
  EXPECT_FALSE(StringUtil::endsWith("ford", "x"));
  EXPECT_FALSE(StringUtil::endsWith("ford", "fnord"));
  EXPECT_TRUE(StringUtil::endsWith("fnord", "fnord"));
  EXPECT_FALSE(StringUtil::endsWith("fnord", "fnordbar"));
});

TEST_CASE(StringUtilTest, TestHexPrint, [] () {
  auto data1 = "\x17\x23\x42\x01";
  EXPECT_EQ(StringUtil::hexPrint(data1, 4), "17 23 42 01");
  EXPECT_EQ(StringUtil::hexPrint(data1, 4, false), "17234201");
  EXPECT_EQ(StringUtil::hexPrint(data1, 4, true, true), "01 42 23 17");
  EXPECT_EQ(StringUtil::hexPrint(data1, 4, false, true), "01422317");
});

TEST_CASE(StringUtilTest, TestReplaceAll, [] () {
  std::string str =
      "cloud computing, or in simpler shorthand just >the cloud<...";

  StringUtil::replaceAll(&str, "cloud", "butt");
  EXPECT_EQ(str, "butt computing, or in simpler shorthand just >the butt<...");

  StringUtil::replaceAll(&str, "butt", "");
  EXPECT_EQ(str, " computing, or in simpler shorthand just >the <...");
});
