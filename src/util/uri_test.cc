/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include "uri.h"
#include "runtimeexception.h"
#include "unittest.h"

using namespace fnordmetric::util;

UNIT_TEST(URITest);

TEST_CASE(URITest, TestSchemeAndAuthority, [] () {
  URI uri("fnord://myhost");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 0);
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithPort, [] () {
  URI uri("fnord://myhost:2345");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 2345);
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfo, [] () {
  URI uri("fnord://blah@myhost");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 0);
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfoWithPort, [] () {
  URI uri("fnord://blah@myhost:2345");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 2345);
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfoSub, [] () {
  URI uri("fnord://blah:asd@myhost");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah:asd");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 0);
});

TEST_CASE(URITest, TestSchemeAndAuthorityWithUserInfoSubWithPort, [] () {
  URI uri("fnord://blah:asd@myhost:2345");

  EXPECT_EQ(uri.scheme(), "fnord");
  EXPECT_EQ(uri.userinfo(), "blah:asd");
  EXPECT_EQ(uri.host(), "myhost");
  EXPECT_EQ(uri.port(), 2345);
});
